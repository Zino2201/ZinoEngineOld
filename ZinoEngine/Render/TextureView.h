#pragma once

#include "Render/DeviceResource.h"

class ITexture;
class ISampler;

struct STextureViewInfo
{
	ITexture* Texture;
	ETextureViewType ViewType;
	EFormat Format;
	uint32_t MipLevels;

	STextureViewInfo(ITexture* InTexture, const ETextureViewType& InViewType,
		const EFormat& InFormat, const uint32_t& InMipLevels = 1) : 
		Texture(InTexture), ViewType(InViewType), Format(InFormat), MipLevels(InMipLevels) {}
};

class ITextureView : public IDeviceResource
{
public:
	ITextureView(const STextureViewInfo& InInfos) {}
	virtual ~ITextureView() = default;

	virtual void SetSampler(const std::shared_ptr<ISampler>& InSampler) = 0;
	virtual ISampler* GetSampler() const = 0;
};