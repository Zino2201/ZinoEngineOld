#include "FrameGraph.h"
#include <stack>
#include <unordered_set>
#include <algorithm>
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemContext.h"

DECLARE_LOG_CATEGORY(FrameGraph);

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

	/**
	 * Used to build a subpass as they are not (yet) supported
	 */
	std::vector<size_t> ColorAttachmentsIdx;
	std::vector<size_t> DepthAttachmentsIdx;

	/**
	 * Iterate over textures to find render targets/depth stencils
	 */
	size_t Idx = 0;
	for(const auto& Texture : InRenderPass.Textures)
	{
		SRSRenderPassAttachment Attachment;
		Attachment.Format = Texture.TextureInfos.Format;
		Attachment.SampleCount = Texture.TextureInfos.SampleCount;
		
		// TODO: Allow a way to specify Clear
		Attachment.Load = ERSRenderPassAttachmentLoadOp::DontCare;
		Attachment.Store = ERSRenderPassAttachmentStoreOp::Store;

		if (HAS_FLAG(Texture.TextureInfos.Usage, ERSTextureUsage::RenderTarget))
		{
			/**
			 * If it's an input, then it is already in the correct layout
			 */
			Attachment.InitialLayout = 
				InRenderPass.IsInput(Texture) ? ERSRenderPassAttachmentLayout::ColorAttachment
				: ERSRenderPassAttachmentLayout::Undefined;

			auto WriteInfos = InRenderPass.GetWriteInfos(Texture);
			if(WriteInfos.has_value())
				Attachment.FinalLayout = WriteInfos->FinalLayout;
			else
				Attachment.FinalLayout = ERSRenderPassAttachmentLayout::ColorAttachment;

			ColorAttachmentsIdx.push_back(Idx++);
			Pass.ColorAttachments.emplace_back(Attachment);
		} 
		else if (HAS_FLAG(Texture.TextureInfos.Usage, ERSTextureUsage::DepthStencil))
		{
			/**
			 * If it's an input, then it is already in the correct layout
			 */
			Attachment.InitialLayout = 
				InRenderPass.IsInput(Texture) ? ERSRenderPassAttachmentLayout::DepthStencilAttachment
				: ERSRenderPassAttachmentLayout::Undefined;
			
			auto WriteInfos = InRenderPass.GetWriteInfos(Texture);
			if (WriteInfos.has_value())
				Attachment.FinalLayout = WriteInfos->FinalLayout;
			else
				Attachment.FinalLayout = ERSRenderPassAttachmentLayout::DepthStencilAttachment;

			DepthAttachmentsIdx.push_back(Idx++);
			Pass.DepthAttachments.emplace_back(Attachment);
		}
	}
	
	/** Subpasses */
	SRSRenderPassSubpass Subpass;
	for(const auto& ColorAttachmentIdx : ColorAttachmentsIdx)
	{
		SRSRenderPassSubpassAttachmentRef Ref;
		Ref.Index = static_cast<uint32_t>(ColorAttachmentIdx);
		Ref.Layout = ERSRenderPassAttachmentLayout::ColorAttachment;
		Subpass.ColorAttachmentRefs.push_back(Ref);
	}

	for (const auto& DepthAttachmentIdx : DepthAttachmentsIdx)
	{
		SRSRenderPassSubpassAttachmentRef Ref;
		Ref.Index = static_cast<uint32_t>(DepthAttachmentIdx);
		Ref.Layout = ERSRenderPassAttachmentLayout::DepthStencilAttachment;
		Subpass.DepthAttachmentRefs.push_back(Ref);
	}

	Pass.Subpasses.push_back(Subpass);

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
	for(const auto& Idx : ExecutionLayout)
	{
		auto& RenderPass = RenderPasses[Idx];

		/**
		 * Build framebuffer
		 */
		SRSFramebuffer Framebuffer;

		size_t Idx = 0;
		size_t DIdx = 0;
		for(const auto& ReadInfo : RenderPass->ReadInfos)
		{
			CRSTexture* Tex = TextureResourceMap[ReadInfo.Target.ID].get();
			if (HAS_FLAG(ReadInfo.Target.TextureInfos.Usage, ERSTextureUsage::DepthStencil))
			{
				Framebuffer.DepthRTs[DIdx++] = Tex;
			}
			else
			{
				Framebuffer.ColorRTs[Idx++] = Tex;
			}
		}

		for (const auto& WriteInfo : RenderPass->WriteInfosMap)
		{
			CRSTexture* Tex = TextureResourceMap[WriteInfo.Target.ID].get();
			if (HAS_FLAG(WriteInfo.Target.TextureInfos.Usage, ERSTextureUsage::DepthStencil))
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
			{ 0, 0, 0, 1 });
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

}