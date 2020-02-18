#pragma once

#include "Core/Asset.h"
#include "RenderCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

/**
 * A texture resource
 */
class CTexture : public CRenderResource
{
public:
	virtual void Init(const uint32_t& InWidth, const uint32_t& InHeight,
		const uint32_t& InChannels, uint8_t* InData);

	CRenderSystemTextureView* GetTextureView() const { return TextureView.get(); }
protected:
	virtual void InitRenderThread() override;
	virtual void DestroyRenderThread() override;
private:
	uint32_t Width;
	uint32_t Height;
	uint32_t Channels;
	uint8_t* Data;
	CRenderSystemTexturePtr Texture;
	CRenderSystemTextureViewPtr TextureView;
	CRenderSystemSamplerPtr Sampler;
};

class CTexture2D : public IAsset
{
public:
	~CTexture2D();

	virtual void Load(const std::string& InPath) override;

	CTexture* GetResource() const { return Resource; }
private:
	uint32_t Width;
	uint32_t Height;
	uint32_t Channels;
	CTexture* Resource;
};