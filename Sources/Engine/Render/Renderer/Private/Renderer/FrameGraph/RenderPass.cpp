#include "RenderPass.h"
#include "FrameGraph.h"
#include "Render/RenderSystem/RenderSystem.h"

namespace ZE::Renderer
{

void CRenderPass::Read(const SRenderPassResource& InResource)
{
	ReadInfos.emplace_back(SRenderPassReadInfos(InResource));
}

void CRenderPass::Write(const SRenderPassResource& InResource, 
	ERSRenderPassAttachmentLayout InFinalLayout)
{
	WriteInfosMap.emplace_back(SRenderPassWriteInfos(InResource, InFinalLayout));
}

SRenderPassResource& CRenderPass::CreateTexture(const SRenderPassTextureInfos& InInfos)
{
	SRenderPassResource& Tex = Graph.CreateResource(ERenderPassResourceType::Texture);
	Tex.TextureInfos = InInfos;

	/** Test if format is a depth format */
	switch(Tex.TextureInfos.Format)
	{
	case EFormat::D32Sfloat:
	case EFormat::D32SfloatS8Uint:
	case EFormat::D24UnormS8Uint:
		Tex.TextureInfos.Usage |= ERSTextureUsage::DepthStencil;
		break;
	default:
		Tex.TextureInfos.Usage |= ERSTextureUsage::RenderTarget;
		break;
	}

	Textures.push_back(Tex);

	return Tex;
}

SRenderPassResource& CRenderPass::CreateRetainedTexture(CRSTexture* InTexture)
{
	SRenderPassResource& Tex = Graph.CreateResource(ERenderPassResourceType::Texture);
	SRenderPassTextureInfos Infos;
	Infos.Width = InTexture->GetWidth();
	Infos.Height = InTexture->GetHeight();
	Infos.Usage = InTexture->GetTextureUsage();
	Infos.Type = InTexture->GetType();
	Infos.Format = InTexture->GetFormat();
	Tex.TextureInfos = Infos;
	Tex.bIsRetained = true;
	Textures.push_back(Tex);
	Graph.TextureResourceMap[Tex.ID] = InTexture;

	return Tex;
}

std::optional<SRenderPassWriteInfos> CRenderPass::GetWriteInfos(
	const SRenderPassResource& InResource) const
{
	for (const auto& WriteInfo : WriteInfosMap)
	{
		if (WriteInfo.Target == InResource)
			return std::make_optional(WriteInfo);
	}

	return std::nullopt;
}

bool CRenderPass::IsInput(const SRenderPassResource& InResource) const
{
	for(const auto& ReadInfo : ReadInfos)
	{
		if(ReadInfo.Target == InResource)
			return true;
	}

	return false;
}

bool CRenderPass::IsOutput(const SRenderPassResource& InResource) const
{
	for (const auto& WriteInfo : WriteInfosMap)
	{
		if (WriteInfo.Target == InResource)
			return true;
	}

	return false;
}

/** RESOURCE MANAGER */
CRSTexture* CRenderPassPersistentResourceManager::GetOrCreateTexture(
	const uint32_t& InID, const SRenderPassTextureInfos& InInfos)
{
	auto& Find = TextureMap.find(InID);
	if(Find != TextureMap.end())
	{
		if(SRenderPassTextureInfosHash()(Find->second.Infos) 
			!= SRenderPassTextureInfosHash()(InInfos))
		{
			SEntry Entry;

			CRSTexture* Texture = GRenderSystem->CreateTexture(
				InInfos.Type,
				InInfos.Usage,
				InInfos.MemoryUsage,
				InInfos.Format,
				InInfos.Width,
				InInfos.Height,
				InInfos.Depth,
				InInfos.ArraySize,
				InInfos.MipLevels,
				InInfos.SampleCount,
				SRSResourceCreateInfo(nullptr, "FrameGraph Texture"));

			Entry.Infos = InInfos;
			Entry.Texture = Texture;

			TextureMap[InID] = Entry;
			
			return Texture;
		}

		Find->second.InactiveCounter = 0;

		return Find->second.Texture.get();
	}

	SEntry Entry;

	CRSTexture* Texture = GRenderSystem->CreateTexture(
		InInfos.Type,
		InInfos.Usage,
		InInfos.MemoryUsage,
		InInfos.Format,
		InInfos.Width,
		InInfos.Height,
		InInfos.Depth,
		InInfos.ArraySize,
		InInfos.MipLevels,
		InInfos.SampleCount,
		SRSResourceCreateInfo(nullptr, "FrameGraph Texture"));

	Entry.Infos = InInfos;
	Entry.Texture = Texture;

	TextureMap.insert({ InID, Entry });

	return Texture;
}

void CRenderPassPersistentResourceManager::UpdateLifetimes()
{
	std::vector<uint32_t> TexturesToDelete;

	for(auto& [ID, Infos] : TextureMap)
	{
		Infos.InactiveCounter++;

		if(Infos.InactiveCounter >= GTextureLifetime)
			TexturesToDelete.emplace_back(ID);
	}

	for(const auto& Texture : TexturesToDelete)
		TextureMap.erase(Texture);
}

}