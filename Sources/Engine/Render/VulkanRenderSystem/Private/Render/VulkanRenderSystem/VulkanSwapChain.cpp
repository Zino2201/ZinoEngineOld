#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanQueue.h"
#include "Render/VulkanRenderSystem/VulkanTexture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

CVulkanSwapChain::CVulkanSwapChain(CVulkanDevice* InDevice,
	const uint32_t& InWidth,
	const uint32_t& InHeight,
	const vk::SurfaceKHR& InSurface)
	: CVulkanDeviceResource(InDevice),
	Surface(InSurface), CurrentImageIdx(0), CurrentFrame(0)
{
	/**
	 * Query details about swap chain
	 */
	SVulkanSwapChainSupportDetails Details = VulkanUtil::QuerySwapChainDetails(
		Device->GetPhysicalDevice(),
		Surface);

	SVulkanQueueFamilyIndices Indices =
		VulkanUtil::GetQueueFamilyIndices(Device->GetPhysicalDevice());

	uint32_t QueueFamilyIndices[] = { Indices.Graphics.value() };

	SurfaceFormat = ChooseSwapChainFormat(Details.Formats);
	vk::PresentModeKHR PresentMode = ChooseSwapChainPresentMode(Details.PresentModes);
	Extent = ChooseSwapChainExtent(Details.Capabilities, InWidth, InHeight);

	ImageCount = Details.Capabilities.minImageCount + 1;
	if (Details.Capabilities.maxImageCount > 0 &&
		ImageCount > Details.Capabilities.maxImageCount)
		ImageCount = Details.Capabilities.maxImageCount;

	if(!Device->GetPhysicalDevice().getSurfaceSupportKHR(
		Device->GetPresentQueue()->GetFamilyIndex(), Surface).value)
		LOG(ELogSeverity::Fatal, VulkanRS, "Selected present queue doesn't support presentation !");

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
		Device->GetGraphicsQueue() != Device->GetPresentQueue() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
		Device->GetGraphicsQueue() != Device->GetPresentQueue() ? 2 : 0,
		Device->GetGraphicsQueue() != Device->GetPresentQueue() ? QueueFamilyIndices : nullptr,
		Details.Capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		PresentMode,
		VK_TRUE,
		vk::SwapchainKHR() /* Old swapchain */);
	Swapchain = Device->GetDevice().createSwapchainKHRUnique(CreateInfos).value;
	if (!Swapchain)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create swap chain");

	/** Get swapchain images */
	Images = Device->GetDevice().getSwapchainImagesKHR(*Swapchain).value;

	/** Create swapchain image views */
	ImageViews.resize(Images.size());

	for (int i = 0; i < Images.size(); ++i)
	{
		ImageViews[i] = new CVulkanTexture(InDevice,
			ERSTextureType::Tex2D,
			ERSTextureUsage::RenderTarget,
			ERSMemoryUsage::DeviceLocal,
			VulkanUtil::VkFormatToFormat(SurfaceFormat.format),
			InWidth,
			InHeight,
			1,
			1,
			1,
			ESampleCount::Sample1,
			Images[i],
			{});

		if(!ImageViews[i])
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create swap chain image view for image %i", i);
	}

	/** Create semaphores */
	{
		vk::SemaphoreCreateInfo CreateInfo;

		ImageAcquired = Device->GetDevice().createSemaphoreUnique(CreateInfo).value;
		if (!ImageAcquired)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create image acquired semaphore");

		RenderFinished = Device->GetDevice().createSemaphoreUnique(CreateInfo).value;
		if (!RenderFinished)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create render finished semaphore");
	}
}

void CVulkanSwapChain::AcquireImage()
{
	/** Acquire new image */
	auto [Result, ImageIdx] = Device->GetDevice().acquireNextImageKHR(
		*Swapchain,
		std::numeric_limits<uint64_t>::max(),
		*ImageAcquired,		
		nullptr);

	CurrentImageIdx = ImageIdx;
	
	/** If swap chain if out of date, recreate it */
	if(Result == vk::Result::eErrorOutOfDateKHR)
	{

	}
}

void CVulkanSwapChain::Present(CVulkanQueue* InPresentQueue)
{
	/** Present to the present queue */
	vk::PresentInfoKHR PresentInfo(
		1,
		&*RenderFinished,
		1,
		&*Swapchain,
		&CurrentImageIdx);

	vk::Result Result = InPresentQueue->GetQueue().presentKHR(PresentInfo);

	/** If swap chain is out of date or suboptimal, recreate it */
	if (Result == vk::Result::eErrorOutOfDateKHR ||
		Result == vk::Result::eSuboptimalKHR)
	{
	}
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