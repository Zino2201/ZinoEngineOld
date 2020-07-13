#pragma once

#include "VulkanCore.h"

class CVulkanQueue;
class CVulkanSurface;

/**
 * Vulkan swap chain
 */
class CVulkanSwapChain : public CVulkanDeviceResource
{
public:
    CVulkanSwapChain(CVulkanDevice* InDevice,
        const uint32_t& InWidth,
        const uint32_t& InHeight,
        CVulkanSurface* InSurface,
        const bool& bInUseVsync,
        const vk::SwapchainKHR& InOldSwapchain = vk::SwapchainKHR());
    ~CVulkanSwapChain();

    vk::Result AcquireImage();
    vk::Result Present(CVulkanQueue* InPresentQueue);

    /**
     * Acquire ownership of swap chain handle
     * Used for recreation of swap chain
     */
    vk::SwapchainKHR AcquireHandle() { return Swapchain.release(); }

    const vk::Semaphore& GetImageAcquiredSemaphore() const { return *ImageAcquired; }
    const vk::Semaphore& GetRenderFinishedSemaphore() const { return *RenderFinished; }
    const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return SurfaceFormat; }
    CRSTexture* GetBackbufferTexture() const { return ImageViews[CurrentImageIdx].get(); }
    const vk::SwapchainKHR& GetSwapChain() const { return Swapchain.get(); }
    const uint32_t& GetCurrentImageIdx() const { return CurrentImageIdx; }
private:
	vk::SurfaceFormatKHR ChooseSwapChainFormat(const std::vector<vk::SurfaceFormatKHR>& InFormats) const;
	vk::PresentModeKHR ChooseSwapChainPresentMode(const std::vector<vk::PresentModeKHR>& InModes) const;
	vk::Extent2D ChooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR& InCapabilities,
		const uint32_t& InWidth,
		const uint32_t& InHeight) const;
private:
    vk::UniqueSwapchainKHR Swapchain;

    vk::SurfaceKHR Surface;
    
    uint32_t Width;
    uint32_t Height;

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
