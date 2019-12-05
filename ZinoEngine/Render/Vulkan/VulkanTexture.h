#pragma once

#include "VulkanDeviceResource.h"
#include "Render/Texture.h"

class CVulkanTexture : public ITexture,
	public CVulkanDeviceResource
{
public:
	CVulkanTexture(CVulkanDevice* InDevice, const STextureInfo& InInfos);
	~CVulkanTexture();

	virtual void Copy(IBuffer* InSrc) override;

	void TransitionImageLayout(vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout);

	const vk::Image& GetImage() const { return Image; }
	virtual const STextureInfo& GetInfo() const override { return Infos; }
private:
	void GenerateMipmaps();
private:
	vk::Image Image;
	VmaAllocation Allocation;
	STextureInfo Infos;
};