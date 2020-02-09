#pragma once

#include "Core/Asset.h"
#include "RenderCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CTexture2D : public IAsset
{
public:
	virtual void Load(const std::string& InPath) override;

	CRenderSystemTextureView* GetTextureView() const { return TextureView.get(); }
private:
	uint32_t Width;
	uint32_t Height;
	uint32_t Channels;
	CRenderSystemTexturePtr Texture;
	CRenderSystemTextureViewPtr TextureView;
	CRenderSystemSamplerPtr Sampler;
};