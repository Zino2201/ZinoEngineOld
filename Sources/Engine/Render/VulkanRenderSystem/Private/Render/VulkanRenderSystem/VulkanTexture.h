#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

/**
 * Base class for a vulkan texture
 */
class CVulkanTexture : public CVulkanDeviceResource,
    public CRSTexture
{
public:
    /**
     * Basic ctor, used for creating a texture + view
     */
    CVulkanTexture(CVulkanDevice* InDevice,
        const ERSTextureType& InTextureType,
        const ERSTextureUsage& InTextureUsage,
        const ERSMemoryUsage& InMemoryUsage,
        const EFormat& InFormat,
        uint32_t InWidth,
        uint32_t InHeight,
        uint32_t InDepth,
        uint32_t InArraySize,
        uint32_t InMipLevels,
        const ESampleCount& InSampleCount,
        const SRSResourceCreateInfo& InCreateInfo);

    /**
     * Ctor for only created a view from a vk::Image
     * (Doesn't destroy the vkImage !)
     */
    CVulkanTexture(CVulkanDevice* InDevice, 
		const ERSTextureType& InTextureType,
		const ERSTextureUsage& InTextureUsage,
		const ERSMemoryUsage& InMemoryUsage,
		const EFormat& InFormat,
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth,
		const uint32_t& InArraySize,
		const uint32_t& InMipLevels,
		const ESampleCount& InSampleCount,
        const vk::Image& InImage,
        const SRSResourceCreateInfo& InCreateInfo);
    virtual ~CVulkanTexture();

    const vk::ImageView& GetImageView() const { return *ImageView; }
protected:
    bool bShouldDestroyImage;
    vk::Image Image;
    vk::UniqueImageView ImageView;
    VmaAllocation Allocation;
};