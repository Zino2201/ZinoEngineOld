#pragma once

#include "VulkanCore.h"

class CVulkanQueue;

/**
 * Vulkan swap chain
 */
class CVulkanSwapChain
{
public:
	CVulkanSwapChain();
	~CVulkanSwapChain();

	void AcquireImage();
	void Present(CVulkanQueue* InPresentQueue);

	const std::vector<vk::UniqueImageView>& GetImageViews() const { return ImageViews; }
	const vk::Extent2D& GetExtent() const { return Extent; }
	const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return SurfaceFormat; }
	const vk::Semaphore& GetImageAvailableSemaphore() const { return *ImageAvailableSemaphores[CurrentFrame]; }
	const vk::Semaphore& GetRenderFinishedSemaphore() const { return *RenderFinishedSemaphores[CurrentFrame]; }
	const uint32_t& GetCurrentImageIndex() const { return CurrentImageIndex; }
	const uint32_t GetImageCount() const { return static_cast<uint32_t>(Images.size()); }
	const uint32_t GetCurrentFrame() const { return CurrentFrame; }
	const vk::Fence& GetFenceForCurrentFrame() const { return *InFlightFences[CurrentFrame]; }
private:
	void Create();
	void OnSwapChainOutOfDate();
	void OnWindowResized();
	vk::SurfaceFormatKHR ChooseSwapChainFormat(const std::vector<vk::SurfaceFormatKHR>& InFormats) const;
	vk::PresentModeKHR ChooseSwapChainPresentMode(const std::vector<vk::PresentModeKHR>& InModes) const;
	vk::Extent2D ChooseSwapChainExtent(const vk::SurfaceCapabilitiesKHR& InCapabilities) const;
private:
	/** Swapchain */
	vk::UniqueSwapchainKHR SwapChain;

	/** Image count */
	uint32_t ImageCount;

	/** Swap chain images */
	std::vector<vk::Image> Images;

	/** Swap chain image views */
	std::vector<vk::UniqueImageView> ImageViews;

	/** Swap chain extent */
	vk::Extent2D Extent;

	/** Swap chain surface format */
	vk::SurfaceFormatKHR SurfaceFormat;

	/** Semaphore when new image is available per frame in flight */
	std::vector<vk::UniqueSemaphore> ImageAvailableSemaphores;

	/** Semaphore when render is finished per frame in flight */
	std::vector<vk::UniqueSemaphore> RenderFinishedSemaphores;

	/** Per frame in flight fence */
	std::vector<vk::UniqueFence> InFlightFences;

	/** Imagee in flight fence */
	std::vector<vk::Fence> ImagesInFlight;

	/** Current image index */
	uint32_t CurrentImageIndex;

	/** Current frame */
	uint32_t CurrentFrame;

	/** Should recreate swap chain */
	bool bShouldRecreate;
public: /* Delegates*/
	TMulticastDelegate<> OnSwapChainRecreated;
};