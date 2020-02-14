#include "Texture2D.h"
#include <stb_image.h>
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Core/Engine.h"

void CTexture::Init(const uint32_t& InWidth, const uint32_t& InHeight,
	const uint32_t& InChannels, uint8_t* InData)
{
	Width = InWidth;
	Height = InHeight;
	Channels = InChannels;
	Data = InData;
}

void CTexture::InitRenderThread()
{
	uint64_t ImageSize = Width * Height * 4;
	uint32_t MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(Width, Height)))) + 1;

	/** Staging buffer */
	CRenderSystemBufferPtr StagingBuffer = g_Engine->GetRenderSystem()->CreateBuffer(
		SRenderSystemBufferInfos(ImageSize, EBufferUsage::TransferSrc, EBufferMemoryUsage::CpuToGpu));

	/** Copy pixels to buffer */
	void* StagingData = StagingBuffer->Map();
	memcpy(StagingData, Data, static_cast<size_t>(ImageSize));
	StagingBuffer->Unmap();

	delete Data;

	/** Create texture */
	Texture = g_Engine->GetRenderSystem()->CreateTexture(
		SRenderSystemTextureInfo(ETextureType::Texture2D, EFormat::R8G8B8A8UNorm,
			ETextureUsage::TransferSrc | ETextureUsage::TransferDst | ETextureUsage::Sampled,
			ETextureMemoryUsage::GpuOnly,
			Width,
			Height,
			1,
			MipLevels));

	/** Copy buffer to texture */
	Texture->Copy(StagingBuffer.get());

	/** Create texture view */
	TextureView = g_Engine->GetRenderSystem()->CreateTextureView(
		SRenderSystemTextureViewInfo(Texture.get(), ETextureViewType::ShaderResource, EFormat::R8G8B8A8UNorm,
			MipLevels));

	/** Create a sampler */
	Sampler = g_Engine->GetRenderSystem()->CreateSampler(SRenderSystemSamplerInfo(
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
		static_cast<float>(MipLevels)));

	TextureView->SetSampler(Sampler.get());
}

void CTexture::DestroyRenderThread()
{
	
}

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

	/** Initialize resource */
	Resource = std::make_unique<CTexture>();
	Resource->Init(Width, Height, Channels, Pixels);
	Resource->InitResources();
}