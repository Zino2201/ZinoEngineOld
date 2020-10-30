#pragma once

#include "Vulkan.h"
#include "Gfx/Backend.h"

namespace ze::gfx::vulkan
{

class Device;
class Queue;

class SwapChain
{
public:
	static constexpr uint8_t max_image_count = 3;

	SwapChain(Device& in_device,
		const vk::SurfaceKHR& in_surface,
		const uint32_t in_width,
		const uint32_t in_height);

	vk::Result create(const uint32_t in_width,
		const uint32_t in_height);

	vk::Result acquire_image();
	vk::Result present(Queue& in_queue, 
		const std::vector<vk::Semaphore>& in_wait_semaphores);

	static SwapChain* get(const ResourceHandle& in_handle);

	ZE_FORCEINLINE bool is_valid() const 
	{ 
		return swapchain && !images.empty() && !image_views.empty() && images.size() == image_views.size(); 
	}

	ZE_FORCEINLINE vk::SwapchainKHR& get_swapchain() { return *swapchain; }
	ZE_FORCEINLINE ResourceHandle get_backbuffer() { return *image_views[current_image]; }
private:
	vk::SurfaceFormatKHR choose_swapchain_format(const std::vector<vk::SurfaceFormatKHR>& in_formats) const;
	vk::PresentModeKHR choose_present_mode(const std::vector<vk::PresentModeKHR>& in_present_modes) const;
	vk::Extent2D choose_extent(const vk::SurfaceCapabilitiesKHR& in_capabilities,
		const uint32_t& in_width,
		const uint32_t& in_height) const;
private:
	Device& device;
	vk::SurfaceKHR surface;
	vk::UniqueSwapchainKHR swapchain;
	vk::Extent2D extent;
    uint32_t image_count;
    uint32_t current_image;
    uint32_t current_frame;
	vk::SurfaceFormatKHR format;
	std::vector<UniqueTexture> images;
	std::vector<UniqueTextureView> image_views;
	vk::UniqueSemaphore image_ready;
};

}