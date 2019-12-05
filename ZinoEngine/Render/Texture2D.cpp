#include "Texture2D.h"
#include <stb_image.h>
#include "Buffer.h"
#include "RenderSystem.h"
#include "Core/Engine.h"
#include "Render/Texture.h"
#include "Render/TextureView.h"
#include "Render/Sampler.h"

void CTexture2D::Load(const std::string& InPath)
{
	stbi_uc* Pixels = stbi_load(InPath.c_str(),
		reinterpret_cast<int*>(&Width), 
		reinterpret_cast<int*>(&Height), 
		reinterpret_cast<int*>(&Channels), STBI_rgb_alpha);

	if(!Pixels)
	{
		// TODO: Load error mat ?
		LOG(ELogSeverity::Fatal, "Failed to load texture %s", InPath.c_str())
		return;
	}

	uint64_t ImageSize = Width * Height * 4;

	/** Staging buffer */
	std::shared_ptr<IBuffer> StagingBuffer = CEngine::Get().GetRenderSystem()->CreateBuffer(
		SBufferInfos(ImageSize, EBufferUsage::TransferSrc, EBufferMemoryUsage::CpuToGpu));

	/** Copy pixels to buffer */
	void* StagingData = StagingBuffer->Map();
	memcpy(StagingData, Pixels, static_cast<size_t>(ImageSize));
	StagingBuffer->Unmap();

	/** Free pixels */
	stbi_image_free(Pixels);

	/** Create texture */
	Texture = CEngine::Get().GetRenderSystem()->CreateTexture(
		STextureInfo(ETextureType::Texture2D, EFormat::R8G8B8A8UNorm,
			ETextureUsage::TransferDst | ETextureUsage::Sampled,
			ETextureMemoryUsage::GpuOnly,
			Width,
			Height));

	/** Copy buffer to texture */
	StagingBuffer->Copy(Texture.get());

	/** Create texture view */
	TextureView = CEngine::Get().GetRenderSystem()->CreateTextureView(
		STextureViewInfo(Texture.get(), ETextureViewType::TextureView2D, EFormat::R8G8B8A8UNorm));

	/** Create a sampler */
	Sampler = CEngine::Get().GetRenderSystem()->CreateSampler(SSamplerInfo(
		ESamplerFilter::Linear,
		ESamplerFilter::Linear,
		ESamplerFilter::Linear,
		ESamplerAddressMode::Repeat,
		ESamplerAddressMode::Repeat,
		ESamplerAddressMode::Repeat,
		16.f,
		EComparisonOp::Always,
		0.f,
		0.f,
		0.f));
	
	TextureView->SetSampler(Sampler);
}