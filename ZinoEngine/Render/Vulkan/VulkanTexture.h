#pragma once

#include "VulkanDeviceResource.h"
#include "Render/Texture.h"

class CVulkanTexture : public ITexture,
	public CVulkanDeviceResource
{
public:
	CVulkanTexture(CVulkanDevice* InDevice, const STextureInfo& InInfos);
	~CVulkanTexture();

	const vk::Image& GetImage() const { return Image; }
	const STextureInfo& GetInfos() const { return Infos; }

	void TransitionImageLayout(vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout);
private:
	vk::Image Image;
	VmaAllocation Allocation;
	STextureInfo Infos;
};