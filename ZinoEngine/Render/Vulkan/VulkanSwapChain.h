#pragma once

#include "VulkanCore.h"

/**
 * Vulkan swap chain
 */
class CVulkanSwapChain
{
public:
	CVulkanSwapChain();
	~CVulkanSwapChain();

	const std::vector<vk::UniqueImageView>& GetImageViews() const { return ImageViews; }
	const vk::Extent2D& GetExtent() const { return Extent; }
	const vk::SurfaceFormatKHR& GetSurfaceFormat() const { return SurfaceFormat; }
private:
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
};