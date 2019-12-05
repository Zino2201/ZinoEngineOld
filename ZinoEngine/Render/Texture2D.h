#pragma once

#include "Core/Asset.h"
#include "RenderCore.h"

class ITexture;
class ITextureView;
class ISampler;

class CTexture2D : public IAsset
{
public:
	virtual void Load(const std::string& InPath) override;

	ITextureView* GetTextureView() const { return TextureView.get(); }
private:
	uint32_t Width;
	uint32_t Height;
	uint32_t Channels;
	std::shared_ptr<ITexture> Texture;
	std::shared_ptr<ITextureView> TextureView;
	std::shared_ptr<ISampler> Sampler;
};