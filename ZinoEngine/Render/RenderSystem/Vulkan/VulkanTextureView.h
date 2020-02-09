#pragma once

#include "VulkanDeviceResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanTexture;
class CVulkanSampler;

class CVulkanTextureView : public CRenderSystemTextureView,
	public CVulkanDeviceResource
{
public:
	CVulkanTextureView(CVulkanDevice* InDevice, const SRenderSystemTextureViewInfo& InInfos);
	~CVulkanTextureView();

	CVulkanTexture* GetTexture() const { return Texture; }
	const vk::ImageView& GetImageView() const { return *ImageView; }
private:
	vk::UniqueImageView ImageView;
	CVulkanTexture* Texture;
};