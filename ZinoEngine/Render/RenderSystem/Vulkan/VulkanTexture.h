#pragma once

#include "VulkanDeviceResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanTexture : public CRenderSystemTexture,
	public CVulkanDeviceResource
{
public:
	CVulkanTexture(CVulkanDevice* InDevice, const SRenderSystemTextureInfo& InInfos);
	~CVulkanTexture();

	virtual void Copy(CRenderSystemBuffer* InSrc) override;

	void TransitionImageLayout(vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout);

	const vk::Image& GetImage() const { return Image; }
	virtual const SRenderSystemTextureInfo& GetInfo() const override { return Infos; }
private:
	void GenerateMipmaps();
private:
	vk::Image Image;
	VmaAllocation Allocation;
	SRenderSystemTextureInfo Infos;
};