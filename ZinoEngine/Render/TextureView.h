#pragma once

#include "Render/DeviceResource.h"

class ITexture;
class ISampler;

struct STextureViewInfo
{
	ITexture* Texture;
	ETextureViewType ViewType;
	EFormat Format;

	STextureViewInfo(ITexture* InTexture, const ETextureViewType& InViewType,
		const EFormat& InFormat) : Texture(InTexture), ViewType(InViewType), Format(InFormat) {}
};

class ITextureView : public IDeviceResource
{
public:
	ITextureView(const STextureViewInfo& InInfos) {}
	virtual ~ITextureView() = default;

	virtual void SetSampler(const std::shared_ptr<ISampler>& InSampler) = 0;
	virtual ISampler* GetSampler() const = 0;
};