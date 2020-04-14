#pragma once

#include "VulkanCore.h"

class CVulkanQueue;

/**
 * Vulkan swap chain
 */
class CVulkanSwapChain : public CVulkanDeviceResource
{
public:
    CVulkanSwapChain(CVulkanDevice* InDevice,
        const uint32_t& InWidth,
        const uint32_t& InHeight,
        const vk::SurfaceKHR& InSurface);

    void AcquireImage();
    void Present(CVulkanQueue* InPresentQueue);

    const vk::Semaphore& GetImageAcquiredSemaphore() const { return *ImageAcquired; }
    const vk::Semaphore& GetRenderFinishedSemaphore() const { return *RenderFinished; }
    const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return SurfaceFormat; }
    CRSTexture* GetBackbufferTexture() const { return ImageViews[CurrentImageIdx].get(); }
private:
	vk::SurfaceFormatKHR ChooseSwapChainFormat(const std::vector<vk::SurfaceFormatKHR>& InFormats) const;
	vk::PresentModeKHR ChooseSwapChainPresentMode(const std::vector<vk::PresentModeKHR>& InModes) const;
	vk::Extent2D ChooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR& InCapabilities,
		const uint32_t& InWidth,
		const uint32_t& InHeight) const;
private:
    vk::UniqueSwapchainKHR Swapchain;

    vk::SurfaceKHR Surface;
    
    /** Image count */
    uint32_t ImageCount;

    /** Current image idx */
    uint32_t CurrentImageIdx;
    
    /** Current frame */
    uint32_t CurrentFrame;

	/** Swap chain extent */
	vk::Extent2D Extent;

	/** Swap chain surface format */
	vk::SurfaceFormatKHR SurfaceFormat;

    /** Swapchain images */
    std::vector<vk::Image> Images;

    /** Swapchain image views */
    std::vector<CRSTexturePtr> ImageViews;

    /** Semaphore called when acquireImageKHR complete */
    vk::UniqueSemaphore ImageAcquired;

    /** Semaphore called when render finished */
    vk::UniqueSemaphore RenderFinished;
};
