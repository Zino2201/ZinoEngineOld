#pragma once

#include "VulkanDeviceResource.h"
#include "Render/TextureView.h"

class CVulkanTexture;
class CVulkanSampler;

class CVulkanTextureView : public ITextureView,
	public CVulkanDeviceResource
{
public:
	CVulkanTextureView(CVulkanDevice* InDevice, const STextureViewInfo& InInfos);
	~CVulkanTextureView();

	virtual void SetSampler(const std::shared_ptr<ISampler>& InSampler) override;
	virtual ISampler* GetSampler() const override;

	CVulkanTexture* GetTexture() const { return Texture; }
	const vk::ImageView& GetImageView() const { return *ImageView; }
	CVulkanSampler* GetVulkanSampler() const { return Sampler.get(); }
private:
	vk::UniqueImageView ImageView;
	CVulkanTexture* Texture;
	std::shared_ptr<CVulkanSampler> Sampler;
};