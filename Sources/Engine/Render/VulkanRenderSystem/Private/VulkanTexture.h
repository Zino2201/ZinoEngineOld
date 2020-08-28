#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/Texture.h"

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
    CVulkanTexture(CVulkanDevice& InDevice,
        const SRSTextureCreateInfo& InCreateInfo);

    /**
     * Ctor for only created a view from a vk::Image
     * (Doesn't destroy the vkImage !)
     */
    CVulkanTexture(CVulkanDevice& InDevice, 
        const SRSTextureCreateInfo& InCreateInfo,
        const vk::Image& InImage);
    virtual ~CVulkanTexture();
    
    void Copy(const void* Src) override;
    void SetName(const char* InName) override;

    const vk::ImageView& GetImageView() const { return ImageView; }
private:
    void CopyFromBuffer(const vk::Buffer& InBuffer);
    void GenerateMipmaps();
    void TransitionImageLayout(const vk::ImageLayout& InOldLayout, 
        const vk::ImageLayout& InNewLayout);
protected:
    bool bShouldDestroyImage;
    vk::Image Image;
    vk::ImageView ImageView;
    VmaAllocation Allocation;
};