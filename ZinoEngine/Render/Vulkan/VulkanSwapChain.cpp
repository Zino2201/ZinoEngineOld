#include "VulkanSwapChain.h"
#include "Core/Engine.h"
#include "Render/Window.h"
#include "VulkanRenderSystem.h"
#include "VulkanDevice.h"

CVulkanSwapChain::CVulkanSwapChain()
{
	SVulkanSwapChainSupportDetails Details = VulkanUtil::QuerySwapChainSupport(
		g_VulkanRenderSystem->GetDevice()->GetPhysicalDevice(),
		g_VulkanRenderSystem->GetSurface());

	SVulkanQueueFamilyIndices Indices = 
		VulkanUtil::GetQueueFamilyIndices(g_VulkanRenderSystem->GetDevice()->GetPhysicalDevice(),
			g_VulkanRenderSystem->GetSurface());

	uint32_t QueueFamilyIndices[] = { Indices.Graphics.value(), Indices.Present.value() };

	SurfaceFormat = ChooseSwapChainFormat(Details.Formats);
	vk::PresentModeKHR PresentMode = ChooseSwapChainPresentMode(Details.PresentModes);
	Extent = ChooseSwapChainExtent(Details.Capabilities);

	ImageCount = Details.Capabilities.minImageCount + 1;
	if (Details.Capabilities.maxImageCount > 0 && 
		ImageCount > Details.Capabilities.maxImageCount)
		ImageCount = Details.Capabilities.maxImageCount;

	/** Create swap chain */
	vk::SwapchainCreateInfoKHR CreateInfos(
		vk::SwapchainCreateFlagsKHR(),
		g_VulkanRenderSystem->GetSurface(),
		ImageCount,
		SurfaceFormat.format,
		SurfaceFormat.colorSpace,
		Extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		Indices.Graphics != Indices.Present ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
		Indices.Graphics != Indices.Present ? 2 : 0,
		Indices.Graphics != Indices.Present ? QueueFamilyIndices : nullptr,
		Details.Capabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		PresentMode,
		VK_TRUE,
		vk::SwapchainKHR() /* Old swapchain */);
	SwapChain = g_VulkanRenderSystem->GetDevice()->GetDevice().createSwapchainKHRUnique(CreateInfos);
	if (!SwapChain)
		LOG(ELogSeverity::Fatal, "Failed to create swap chain");
	
	/** Acquire images */
	Images = g_VulkanRenderSystem->GetDevice()->GetDevice().getSwapchainImagesKHR(*SwapChain);

	/** Create image views */
	ImageViews.resize(Images.size());
	
	for (int i = 0; i < Images.size(); ++i)
	{
		vk::ImageViewCreateInfo ImageView(
			vk::ImageViewCreateFlags(),
			Images[i],
			vk::ImageViewType::e2D,
			SurfaceFormat.format,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
				0, 1, 0, 1));

		ImageViews[i] = g_VulkanRenderSystem->GetDevice()->GetDevice().createImageViewUnique(ImageView);
	}
}

CVulkanSwapChain::~CVulkanSwapChain() {}

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
	vk::SurfaceCapabilitiesKHR& InCapabilities) const
{
	if (InCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return InCapabilities.currentExtent;
	else
	{
		/** Default set current window width & height */
		vk::Extent2D Extent(CEngine::Get().GetWindow()->GetWidth(),
			CEngine::Get().GetWindow()->GetHeight());

		/** Clamp to swap chain capabilities */
		Extent.width = std::clamp(Extent.width, InCapabilities.minImageExtent.width,
			InCapabilities.maxImageExtent.width);
		Extent.height = std::clamp(Extent.height, InCapabilities.minImageExtent.height,
			InCapabilities.maxImageExtent.height);

		return Extent;
	}
}