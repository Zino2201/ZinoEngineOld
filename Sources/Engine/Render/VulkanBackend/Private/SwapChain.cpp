#include "SwapChain.h"
#include "Device.h"
#include "Queue.h"
#include "VulkanBackend.h"
#include <robin_hood.h>
#include "Surface.h"
#include "Texture.h"
#include "VulkanUtil.h"
#include "Sync.h"

namespace ze::gfx::vulkan
{

robin_hood::unordered_map<ResourceHandle, SwapChain> swapchains;

ResourceHandle VulkanBackend::swapchain_create(const SwapChainCreateInfo& in_create_info)
{
	ResourceHandle handle;

	vk::SurfaceKHR surface = get_or_create(in_create_info.window_handle);
	if(!surface)
		return handle;

	SwapChain swapchain(*device,
		surface,
		in_create_info.width,
		in_create_info.height);
	if(swapchain.is_valid())
	{
		handle = create_resource_handle(ResourceType::SwapChain, 
			static_cast<VkSwapchainKHR>(swapchain.get_swapchain()), in_create_info);
		swapchains.insert({ handle, std::move(swapchain)});
	}

	return handle;
}

void VulkanBackend::swapchain_destroy(const ResourceHandle& in_handle)
{
	swapchains.erase(in_handle);
}

bool VulkanBackend::swapchain_acquire_image(const ResourceHandle& in_swapchain)
{
	SwapChain* swapchain = SwapChain::get(in_swapchain);
	ZE_CHECKF(swapchain, "Invalid swapchain given to swapchain_acquire_image")

	vk::Result result = swapchain->acquire_image();
	if(result != vk::Result::eSuccess)
		return false;

	return true;
}

ResourceHandle VulkanBackend::swapchain_get_backbuffer_texture(const ResourceHandle& in_swapchain)
{
	SwapChain* swapchain = SwapChain::get(in_swapchain);
	ZE_CHECKF(swapchain, "Invalid swapchain given to swapchain_get_backbuffer")
	return swapchain->get_backbuffer_texture();
}

void VulkanBackend::swapchain_resize(const ResourceHandle& in_swapchain, const uint32_t in_new_width,
	const uint32_t in_new_height)
{
	SwapChain* swapchain = SwapChain::get(in_swapchain);
	ZE_CHECKF(swapchain, "Invalid swapchain given to swapchain_resize")
	swapchain->create(in_new_width, in_new_height);
}

ResourceHandle VulkanBackend::swapchain_get_backbuffer(const ResourceHandle& in_swapchain)
{
	SwapChain* swapchain = SwapChain::get(in_swapchain);
	ZE_CHECKF(swapchain, "Invalid swapchain given to swapchain_get_backbuffer")
	return swapchain->get_backbuffer();
}

void VulkanBackend::swapchain_present(const ResourceHandle& in_swapchain,
	const std::vector<ResourceHandle>& in_wait_semaphores)
{
	auto swapchain = swapchains.find(in_swapchain);
	if(swapchain != swapchains.end())
	{
		std::vector<vk::Semaphore> wait_semaphores;
		wait_semaphores.reserve(in_wait_semaphores.size());
		for(const auto& handle : in_wait_semaphores)
		{
			Semaphore* semaphore = Semaphore::get(handle);
			ZE_CHECKF(semaphore, "Invalid wait semaphore passed to swapchain_present");
			if(semaphore)
				wait_semaphores.emplace_back(semaphore->get_semaphore());
		}

		swapchain->second.present(*Queue::get(VulkanBackend::get().get_gfx_queue()),
			wait_semaphores);
	}
}

SwapChain::SwapChain(Device& in_device, const vk::SurfaceKHR& in_surface,
	const uint32_t in_width,
	const uint32_t in_height)
	: device(in_device), surface(in_surface), current_image(0), current_frame(0) 
{
	create(in_width, in_height);
}

vk::Result SwapChain::create(const uint32_t in_width,
	const uint32_t in_height)
{
	vk::SwapchainKHR old_swapchain = swapchain.release();
		
	current_image = 0;
	current_frame = 0;
	images.clear();
	image_views.clear();

	/** Choose swapchain details */
	vk::SurfaceCapabilitiesKHR capabilities = device.get_physical_device().getSurfaceCapabilitiesKHR(surface).value;

	format = choose_swapchain_format(device.get_physical_device().getSurfaceFormatsKHR(surface).value);
	vk::PresentModeKHR present_mode = choose_present_mode(device.get_physical_device().getSurfacePresentModesKHR(surface).value);
	extent = choose_extent(capabilities, in_width, in_height);

	image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 &&
		image_count > capabilities.maxImageCount)
		image_count = capabilities.maxImageCount;

	if(!device.get_physical_device().getSurfaceSupportKHR(
		Queue::get(device.get_present_queue())->get_family(), surface).value)
		ze::logger::fatal("Selected present queue doesn't support presentation !");

	uint32_t queue_family_indices[] = { device.get_queue_family_indices().gfx.value() };

	vk::SwapchainCreateInfoKHR swapchain_create_info(
		vk::SwapchainCreateFlagsKHR(),
		surface,
		image_count,
		format.format,
		format.colorSpace,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
		device.get_gfx_queue() != device.get_present_queue() ? vk::SharingMode::eConcurrent 
			: vk::SharingMode::eExclusive,
		device.get_gfx_queue() != device.get_present_queue() ? 2 : 0,
		device.get_gfx_queue() != device.get_present_queue() ? queue_family_indices : nullptr,
		capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		present_mode,
		VK_TRUE,
		old_swapchain);
	auto [result, handle] = device.get_device().createSwapchainKHRUnique(swapchain_create_info);
	if (!handle)
	{
		ze::logger::error("Failed to create swap chain: {}", vk::to_string(result));
		return result;
	}

	swapchain = std::move(handle);

	if(old_swapchain)
		device.get_device().destroySwapchainKHR(old_swapchain);

	/** Create images views */
	std::vector<vk::Image> sw_images = device.get_device().getSwapchainImagesKHR(*swapchain).value;
	images.reserve(sw_images.size());
	image_views.reserve(sw_images.size());
	for (int i = 0; i < sw_images.size(); ++i)
	{
		auto [tex_result, tex] = get_backend().texture_create(
			sw_images[i],
			TextureCreateInfo(
				TextureType::Tex2D,
				MemoryUsage::GpuOnly,
				to_format(format.format),
				extent.width,
				extent.height,
				1,
				0,
				1,
				SampleCountFlagBits::Count1,
				TextureUsageFlagBits::ColorAttachment));
		images.emplace_back(std::move(tex));

		ResourceHandle view = get_backend().texture_view_create(
			sw_images[i], 
			TextureViewCreateInfo(
				ResourceHandle(),
				TextureViewType::Tex2D,
				to_format(format.format),
				TextureSubresourceRange(
					TextureAspectFlagBits::Color,
					0,
					1,
					0,
					1)));
		if(!view)
		{
			ze::logger::error("Failed to create image view for swap chain image {} ({})",
				i, vk::to_string(result));
			return result;
		}

		image_views.emplace_back(std::move(view));
	}

	{
		image_ready = device.get_device().createSemaphoreUnique(vk::SemaphoreCreateInfo());
		if(!image_ready)
		{
			ze::logger::error("Failed to create image ready semaphore");
		}
	}

	return result;
}

vk::Result SwapChain::acquire_image()
{
	auto [result, image] = device.get_device().acquireNextImageKHR(*swapchain,
		std::numeric_limits<uint64_t>::max(),
		*image_ready,
		vk::Fence());

	current_image = image;
	
	return result;
}

vk::Result SwapChain::present(Queue& in_queue,
	const std::vector<vk::Semaphore>& in_wait_semaphores)
{
	std::vector<vk::Semaphore> wait_semaphores(in_wait_semaphores);
	wait_semaphores.emplace_back(*image_ready);

	vk::PresentInfoKHR present_info(
		static_cast<uint32_t>(in_wait_semaphores.size()) + 1,
		wait_semaphores.data(),
		1,
		&*swapchain,
		&current_image);

	return in_queue.get_queue().presentKHR(&present_info);
}

vk::SurfaceFormatKHR SwapChain::choose_swapchain_format(const std::vector<vk::SurfaceFormatKHR>& in_formats) const
{
	for(const vk::SurfaceFormatKHR& format : in_formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Unorm &&
			format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
			return format;
	}

	return in_formats[0];
}

vk::PresentModeKHR SwapChain::choose_present_mode(const std::vector<vk::PresentModeKHR>& in_present_modes) const
{
	for (const vk::PresentModeKHR& mode : in_present_modes)
	{
		if (mode == vk::PresentModeKHR::eMailbox)
			return mode;
	}

	return vk::PresentModeKHR::eFifo;
}

SwapChain* SwapChain::get(const ResourceHandle& in_handle)
{
	auto swapchain = swapchains.find(in_handle);

	if(swapchain != swapchains.end())
		return &swapchain->second;

	return nullptr;
}

vk::Extent2D SwapChain::choose_extent(const vk::SurfaceCapabilitiesKHR& in_capabilities,
	const uint32_t& in_width,
	const uint32_t& in_height) const
{
	if (in_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return in_capabilities.currentExtent;
	else
	{
		/** Default set current window width & height */
		vk::Extent2D extent(
			in_width,
			in_height);

		/** Clamp to swap chain capabilities */
		extent.width = std::clamp(extent.width, in_capabilities.minImageExtent.width,
			in_capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, in_capabilities.minImageExtent.height,
			in_capabilities.maxImageExtent.height);

		return extent;
	}
}

}