#include "FrameGraph.h"
#include <stack>
#include <unordered_set>
#include <algorithm>
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Renderer/RendererModule.h"

namespace ZE::Renderer
{

CFrameGraph::CFrameGraph() 
	: AvailableRenderPassID(0), AvailableResourceID(0), AvailablePhysicalResourceID(0)
{

}

bool CFrameGraph::ValidateRenderPass(const CRenderPass& InRenderPass)
{
	/**
	 * Iterate over textures and check if they have valid flags
	 */
	for(const auto& Texture : InRenderPass.Textures)
	{
		if(HAS_FLAG(Texture.TextureInfos.Usage, ERSTextureUsage::RenderTarget) ||
			HAS_FLAG(Texture.TextureInfos.Usage, ERSTextureUsage::DepthStencil))
		{
		
		}
		else
		{
			LOG(ELogSeverity::Error, FrameGraph,
				"Invalid usage flags for texture %d inside render pass %s",
				Texture.ID,
				InRenderPass.Name.c_str());
		}
	}

	return true;
}

void CFrameGraph::SortRenderPasses(std::vector<size_t>& ExecutionLayout)
{
	/**
	 * Sort render passes execution
	 * Try to insert passes between other dependents passes
	 */
	ExecutionLayout.reserve(RenderPasses.size());

	struct SRenderPassEntry
	{
		size_t Index;
		uint32_t Deps;

		SRenderPassEntry() : Index(0), Deps(0) {}
	};

	std::vector<SRenderPassEntry> Entries;
	Entries.reserve(RenderPasses.size());

	for(size_t i = 0; i < RenderPasses.size(); ++i)
	{
		const auto& RenderPass = RenderPasses[i];
		SRenderPassEntry Entry;
		Entry.Index = i;

		for(const auto& OtherRenderPass : RenderPasses)
		{	
			if(DependOn(*RenderPass.get(), *OtherRenderPass.get()))
			{
				Entry.Deps++;
				RenderPass->Dependencies.push_back(OtherRenderPass.get());
			}
		}

		Entries.emplace_back(Entry);
	}

	std::sort(Entries.begin(), Entries.end(),
		[](const SRenderPassEntry& InLeft, const SRenderPassEntry& InRight) -> bool
	{
		return InLeft.Deps < InRight.Deps;
	});

	for(const auto& Entry : Entries)
		ExecutionLayout.emplace_back(Entry.Index);
}

bool CFrameGraph::DependOn(const CRenderPass& InLeft, const CRenderPass& InRight)
{
	/**
     * Check if Left has inputs related to Right outputs
	 */
	for (const auto& Input : InLeft.ReadInfos)
	{
		for (const auto& Output : InRight.WriteInfosMap)
		{
			if (Input.Target == Output.Target)
				return true;
		}
	}
	
	return false;
}

void CFrameGraph::BuildPhysicalResources(const CRenderPass& InRenderPass)
{
	/** Create textures */
	for (const auto& Texture : InRenderPass.Textures)
	{
		if(Texture.bIsRetained || TextureResourceMap.count(Texture.ID))
			continue;

		TextureResourceMap[Texture.ID] = CRenderPassPersistentResourceManager::Get()
			.GetOrCreateTexture(Texture.ID, Texture.TextureInfos);
	}
}

void CFrameGraph::BuildPhysicalRenderPass(CRenderPass& InRenderPass)
{
	SRSRenderPass Pass;
	
	// TODO: Support multiple subpasses
	SRSRenderPassSubpass Subpass;
	
	uint32_t Idx = 0;
	for (const auto& Resource : InRenderPass.Attachments)
	{
		SRenderPassResource& Texture = ResourceMap[Resource];
		auto& ReadInfos = InRenderPass.GetReadInfos(Texture);
		auto& WriteInfos = InRenderPass.GetWriteInfos(Texture);

		SRSRenderPassAttachment Attachment;
		Attachment.Format = Texture.TextureInfos.Format;
		Attachment.SampleCount = Texture.TextureInfos.SampleCount;

		SRSRenderPassSubpassAttachmentRef SubpassRef;
		SubpassRef.Index = Idx;

		/**
		 * Specify load/store operations for attachment
		 */
		if (ReadInfos.has_value())
			Attachment.Load = ReadInfos->LoadOp;
		else
			Attachment.Load = ERSRenderPassAttachmentLoadOp::Clear;

		if (WriteInfos.has_value())
			Attachment.Store = WriteInfos->StoreOp;
		else
			Attachment.Store = ERSRenderPassAttachmentStoreOp::DontCare;

		/**
		 * Determine layout based on texture usage
		 */
		if (HAS_FLAG(Texture.TextureInfos.Usage, ERSTextureUsage::RenderTarget))
		{
			Attachment.InitialLayout = TextureLastLayout.count(Texture.ID) ?
				TextureLastLayout[Texture.ID] : ERSRenderPassAttachmentLayout::Undefined;

			if (WriteInfos.has_value())
			{
				Attachment.FinalLayout = TexLayoutToRSLayout(Texture.TextureInfos, 
					WriteInfos->FinalLayout);
			}
			else
			{
				/**
				 * If there are not write infos, simply keep the layout
				 */
				Attachment.FinalLayout = Attachment.InitialLayout;
			}

			SubpassRef.Layout = Attachment.InitialLayout == ERSRenderPassAttachmentLayout::Undefined
				? ERSRenderPassAttachmentLayout::ColorAttachment : 
				Attachment.InitialLayout;

			Pass.ColorAttachments.emplace_back(Attachment);
			
			if(WriteInfos.has_value())
				Subpass.ColorAttachmentRefs.emplace_back(SubpassRef);
		}
		else
		{
			Attachment.InitialLayout = TextureLastLayout.count(Texture.ID) ?
				TextureLastLayout[Texture.ID] : ERSRenderPassAttachmentLayout::Undefined;

			if (WriteInfos.has_value())
			{
				Attachment.FinalLayout = TexLayoutToRSLayout(Texture.TextureInfos,
					WriteInfos->FinalLayout);
			}
			else
			{
				Attachment.FinalLayout = Attachment.InitialLayout;
			}

			SubpassRef.Layout = Attachment.InitialLayout == ERSRenderPassAttachmentLayout::Undefined
				? ERSRenderPassAttachmentLayout::DepthStencilAttachment :
				Attachment.InitialLayout;

			Pass.DepthAttachments.emplace_back(Attachment);
			if(WriteInfos.has_value())
				Subpass.DepthAttachmentRefs.emplace_back(SubpassRef);
		}
		
		TextureLastLayout[Texture.ID] = Attachment.FinalLayout;

		/**
		 * If input, add it to subpass input attachment ref
		 */
		if (ReadInfos.has_value())
			Subpass.InputAttachmentRefs.emplace_back(SubpassRef);

		Idx++;
	}

	Pass.Subpasses.emplace_back(Subpass);

	InRenderPass.PhysRenderPass = std::move(Pass);
}

bool CFrameGraph::Compile()
{
	/**
	 * Check if render passes data is valid
	 * E.g: checking if read/write resources are specified as RenderTarget/DepthStencil etc
	 */
	for(const auto& RenderPass : RenderPasses)
	{
		if(!ValidateRenderPass(*RenderPass.get()))
		{
			LOG(ELogSeverity::Error, FrameGraph, 
				"Validation of render pass %s failed ! Frame will be skipped !",
				RenderPass->Name.c_str());
			__debugbreak();
			return false;
		}
	}

	SortRenderPasses(ExecutionLayout);

	/** 
	 * Now compile
	 */
	for(auto& RenderPass : RenderPasses)
	{
		BuildPhysicalResources(*RenderPass.get());
		BuildPhysicalRenderPass(*RenderPass.get());
		// TODO: BARRIERS
	}


	return true;
}

void CFrameGraph::Execute()
{
	for(const auto& CurrentPass : ExecutionLayout)
	{
		auto& RenderPass = RenderPasses[CurrentPass];

		/**
		 * Build framebuffer
		 */
		SRSFramebuffer Framebuffer;

		size_t Idx = 0;
		size_t DIdx = 0;

		for(const auto& TextureID : RenderPass->Attachments)
		{
			CRSTexture* Tex = TextureResourceMap[TextureID].get();
			auto& Resource = ResourceMap[TextureID];

			if (HAS_FLAG(Resource.TextureInfos.Usage, ERSTextureUsage::DepthStencil))
			{
				Framebuffer.DepthRTs[DIdx++] = Tex;
			}
			else
			{
				Framebuffer.ColorRTs[Idx++] = Tex;
			}
		}

 		GRSContext->BeginRenderPass(
			RenderPass->PhysRenderPass,
			Framebuffer,
			{ 0, 0, 0, 1 },
			RenderPass->Name.c_str());
		RenderPass->Execute(GRSContext);
		GRSContext->EndRenderPass();
	}

	/** Update persistent resources lifetimes */
	CRenderPassPersistentResourceManager::Get().UpdateLifetimes();
}

SRenderPassResource& CFrameGraph::CreateResource(ERenderPassResourceType InType)
{
	SRenderPassResource& Resource = ResourceMap[AvailableResourceID];
	Resource = SRenderPassResource(AvailableResourceID, InType);
	AvailableResourceID++;
	return Resource;
}

ERSRenderPassAttachmentLayout CFrameGraph::TexLayoutToRSLayout(
	const SRenderPassTextureInfos& InInfos, ERenderPassResourceLayout InLayout) const
{
	bool bIsDepthStencil = HAS_FLAG(InInfos.Usage, ERSTextureUsage::DepthStencil);

	switch(InLayout)
	{
	default:
		return ERSRenderPassAttachmentLayout::Undefined;
	case ERenderPassResourceLayout::RenderTarget:
		return bIsDepthStencil ? ERSRenderPassAttachmentLayout::DepthStencilAttachment :
			ERSRenderPassAttachmentLayout::ColorAttachment;
	case ERenderPassResourceLayout::ShaderReadOnlyOptimal:
		return bIsDepthStencil ? ERSRenderPassAttachmentLayout::DepthStencilReadOnlyOptimal :
			ERSRenderPassAttachmentLayout::ShaderReadOnlyOptimal;
	case ERenderPassResourceLayout::Present:
		return ERSRenderPassAttachmentLayout::Present;
	}
}

}