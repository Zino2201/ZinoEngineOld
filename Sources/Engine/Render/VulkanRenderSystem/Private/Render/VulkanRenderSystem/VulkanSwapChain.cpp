#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanQueue.h"
#include "Render/VulkanRenderSystem/VulkanTexture.h"
#include "Render/VulkanRenderSystem/VulkanSurface.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

CVulkanSwapChain::CVulkanSwapChain(CVulkanDevice& InDevice,
	const uint32_t& InWidth,
	const uint32_t& InHeight,
	CVulkanSurface* InSurface,
	const bool& bInUseVsync,
	const vk::SwapchainKHR& InOldSwapchain)
	: CVulkanDeviceResource(InDevice),
	Surface(InSurface->GetSurface()), Width(InWidth), Height(InHeight), 
	CurrentImageIdx(0), CurrentFrame(0)
{
	/**
	 * Query details about swap chain
	 */
	SVulkanSwapChainSupportDetails Details = VulkanUtil::QuerySwapChainDetails(
		Device.GetPhysicalDevice(),
		Surface);

	SVulkanQueueFamilyIndices Indices =
		VulkanUtil::GetQueueFamilyIndices(Device.GetPhysicalDevice());

	uint32_t QueueFamilyIndices[] = { Indices.Graphics.value() };

	SurfaceFormat = ChooseSwapChainFormat(Details.Formats);
	vk::PresentModeKHR PresentMode = bInUseVsync ? vk::PresentModeKHR::eFifo 
		: ChooseSwapChainPresentMode(Details.PresentModes);
	Extent = ChooseSwapChainExtent(Details.Capabilities, Width, Height);

	ImageCount = Details.Capabilities.minImageCount + 1;
	if (Details.Capabilities.maxImageCount > 0 &&
		ImageCount > Details.Capabilities.maxImageCount)
		ImageCount = Details.Capabilities.maxImageCount;

	if(!Device.GetPhysicalDevice().getSurfaceSupportKHR(
		Device.GetPresentQueue()->GetFamilyIndex(), Surface).value)
		ZE::Logger::Fatal("Selected present queue doesn't support presentation !");

	/**
	 * Create swapchain handle
	 */
	vk::SwapchainCreateInfoKHR CreateInfos(
		vk::SwapchainCreateFlagsKHR(),
		Surface,
		ImageCount,
		SurfaceFormat.format,
		SurfaceFormat.colorSpace,
		Extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		Device.GetGraphicsQueue() != Device.GetPresentQueue() ? vk::SharingMode::eConcurrent 
			: vk::SharingMode::eExclusive,
		Device.GetGraphicsQueue() != Device.GetPresentQueue() ? 2 : 0,
		Device.GetGraphicsQueue() != Device.GetPresentQueue() ? QueueFamilyIndices : nullptr,
		Details.Capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		PresentMode,
		VK_TRUE,
		InOldSwapchain);
	Swapchain = Device.GetDevice().createSwapchainKHRUnique(CreateInfos).value;
	if (!Swapchain)
		ZE::Logger::Fatal("Failed to create swap chain");

	/** Get swapchain images */
	Images = Device.GetDevice().getSwapchainImagesKHR(*Swapchain).value;

	/** Create swapchain image views */
	ImageViews.resize(Images.size());

	for (int i = 0; i < Images.size(); ++i)
	{
		ImageViews[i] = new CVulkanTexture(Device, {
			ERSTextureType::Tex2D,
			ERSTextureUsageFlagBits::RenderTarget,
			ERSMemoryUsage::DeviceLocal,
			VulkanUtil::VkFormatToFormat(SurfaceFormat.format),
			Width,
			Height,
			1,
			1,
			1,
			ESampleCount::Sample1 },
			Images[i]);

		if(!ImageViews[i])
			ZE::Logger::Fatal("Failed to create swap chain image view for image {}", i);

		ImageViews[i]->SetName("Swapchain Backbuffer");
	}

	/** Create semaphores */
	{
		vk::SemaphoreCreateInfo CreateInfo;

		ImageAcquired = Device.GetDevice().createSemaphoreUnique(CreateInfo).value;
		if (!ImageAcquired)
			ZE::Logger::Fatal("Failed to create image acquired semaphore");

		RenderFinished = Device.GetDevice().createSemaphoreUnique(CreateInfo).value;
		if (!RenderFinished)
			ZE::Logger::Fatal("Failed to create render finished semaphore");
	}
}

CVulkanSwapChain::~CVulkanSwapChain() = default;

vk::Result CVulkanSwapChain::AcquireImage()
{
	/** Acquire new image */
	auto [Result, ImageIdx] = Device.GetDevice().acquireNextImageKHR(
		*Swapchain,
		std::numeric_limits<uint64_t>::max(),
		*ImageAcquired,
		nullptr);

	CurrentImageIdx = ImageIdx;

	return Result;
}

vk::Result CVulkanSwapChain::Present(CVulkanQueue* InPresentQueue)
{
	/** Present to the present queue */
	vk::PresentInfoKHR PresentInfo(
		1,
		&*RenderFinished,
		1,
		&*Swapchain,
		&CurrentImageIdx);

	return InPresentQueue->GetQueue().presentKHR(PresentInfo);
}

SVulkanSwapChainSupportDetails VulkanUtil::QuerySwapChainDetails(
	const vk::PhysicalDevice& InPhysicalDevice,
	const vk::SurfaceKHR& InSurface)
{
	SVulkanSwapChainSupportDetails Details;

	Details.Capabilities = InPhysicalDevice.getSurfaceCapabilitiesKHR(InSurface).value;
	Details.Formats = InPhysicalDevice.getSurfaceFormatsKHR(InSurface).value;
	Details.PresentModes = InPhysicalDevice.getSurfacePresentModesKHR(InSurface).value;

	return Details;
}

vk::SurfaceFormatKHR CVulkanSwapChain::ChooseSwapChainFormat(const
	std::vector<vk::SurfaceFormatKHR>& InFormats) const
{
	for (const vk::SurfaceFormatKHR& Format : InFormats)
	{
		if (Format.format == vk::Format::eB8G8R8A8Unorm &&
			Format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
			return Format;
	}

	return InFormats[0];
}

vk::PresentModeKHR CVulkanSwapChain::ChooseSwapChainPresentMode(const
	std::vector<vk::PresentModeKHR>& InModes) const
{
	for (const vk::PresentModeKHR& Mode : InModes)
	{
		if (Mode == vk::PresentModeKHR::eMailbox)
			return Mode;
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D CVulkanSwapChain::ChooseSwapChainExtent(const
	vk::SurfaceCapabilitiesKHR& InCapabilities,
	const uint32_t& InWidth,
	const uint32_t& InHeight) const
{
	if (InCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return InCapabilities.currentExtent;
	else
	{
		/** Default set current window width & height */
		vk::Extent2D Extent(
			InWidth,
			InHeight);

		/** Clamp to swap chain capabilities */
		Extent.width = std::clamp(Extent.width, InCapabilities.minImageExtent.width,
			InCapabilities.maxImageExtent.width);
		Extent.height = std::clamp(Extent.height, InCapabilities.minImageExtent.height,
			InCapabilities.maxImageExtent.height);

		return Extent;
	}
}