#include "VulkanSwapChain.h"
#include "Core/Engine.h"
#include "Render/Window.h"
#include "VulkanRenderSystem.h"
#include "VulkanDevice.h"
#include "Render/Renderer/ForwardSceneRenderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderCommandContext.h"
#include "VulkanQueue.h"
#include "VulkanPipelineLayout.h"
#include <SDL2/SDL.h>

DECLARE_TIMER_STAT(VulkanRendering, DescriptorSetResetTime);

CVulkanSwapChain::CVulkanSwapChain()
{
	g_Engine->GetWindow()->OnWindowResized.Bind(
		std::bind(&CVulkanSwapChain::OnWindowResized, this));

	Create();
}

CVulkanSwapChain::~CVulkanSwapChain() {}

void CVulkanSwapChain::Create()
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
	SwapChain = g_VulkanRenderSystem->GetDevice()->GetDevice().createSwapchainKHRUnique(CreateInfos).value;
	if (!SwapChain)
		LOG(ELogSeverity::Fatal, "Failed to create swap chain");
	
	/** Acquire images */
	Images = g_VulkanRenderSystem->GetDevice()->GetDevice().getSwapchainImagesKHR(*SwapChain).value;

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

		ImageViews[i] = g_VulkanRenderSystem->GetDevice()->GetDevice()
			.createImageViewUnique(ImageView).value;
	}

	/** Create semaphores and fences */
	//if(InFlightFences.empty())
	{
		vk::SemaphoreCreateInfo CreateInfos;
		vk::FenceCreateInfo FenceCreateInfos(vk::FenceCreateFlagBits::eSignaled);

		for (uint32_t i = 0; i < g_MaxFramesInFlight; ++i)
		{
			ImageAvailableSemaphores.emplace_back(g_VulkanRenderSystem->GetDevice()->GetDevice()
				.createSemaphoreUnique(CreateInfos).value);
			if (!ImageAvailableSemaphores[i])
				LOG(ELogSeverity::Fatal, "Failed to create image available semaphore")

			RenderFinishedSemaphores.emplace_back(g_VulkanRenderSystem->GetDevice()->GetDevice()
				.createSemaphoreUnique(CreateInfos).value);
			if (!RenderFinishedSemaphores[i])
				LOG(ELogSeverity::Fatal, "Failed to create render finished semaphore")

			InFlightFences.emplace_back(g_VulkanRenderSystem->GetDevice()->GetDevice()
				.createFenceUnique(FenceCreateInfos).value);
			if (!InFlightFences[i])
				LOG(ELogSeverity::Fatal, "Failed to create in flight fence")	
		}

		ImagesInFlight.resize(ImageViews.size(), nullptr);
	}
}

void CVulkanSwapChain::OnSwapChainOutOfDate()
{
	bShouldRecreate = false;

	/** Wait GPU */
	g_VulkanRenderSystem->WaitGPU();

	/** First free all existing objects */
	ImageViews.clear();
	Images.clear();
	SwapChain.reset();

	ImagesInFlight.clear();
	ImageAvailableSemaphores.clear();
	RenderFinishedSemaphores.clear();
	InFlightFences.clear();

	/** Recreate all */
	Create();

	/** Broadcast the event */
	OnSwapChainRecreated.Broadcast();

	/** After all of that done, reacquire the image */
	//AcquireImage();
}

void CVulkanSwapChain::OnWindowResized()
{
	bShouldRecreate = true;
}

// TODO: Fix resizing
void CVulkanSwapChain::AcquireImage()
{
	/** Wait for last frame render before acquriring new image */
	g_VulkanRenderSystem->GetDevice()->GetDevice().waitForFences(
		{ GetFenceForCurrentFrame() }, VK_TRUE, std::numeric_limits<uint64_t>::max());

	vk::ResultValue<uint32_t> Result = 
		g_VulkanRenderSystem->GetDevice()->GetDevice().acquireNextImageKHR(
			*SwapChain,
			std::numeric_limits<uint64_t>::max(),
			*ImageAvailableSemaphores[CurrentFrame],
			vk::Fence());

	CurrentImageIndex = Result.value;

	if (Result.result == vk::Result::eErrorOutOfDateKHR)
	{
		/** Swap chain is out of date, recreate it and broadcast the message */
		OnSwapChainOutOfDate();
		return;
	}

	if (ImagesInFlight[CurrentImageIndex]) 
	{
		g_VulkanRenderSystem->GetDevice()->GetDevice().waitForFences(
			{ ImagesInFlight[CurrentImageIndex] }, VK_TRUE, std::numeric_limits<uint64_t>::max());

		SCOPED_TIMER_STAT(DescriptorSetResetTime);
		// TODO: Move this to another place ?
		/** Reset all pipeline layouts from previous frame */
		for (CVulkanPipelineLayout* PipelineLayout : CVulkanPipelineLayout::PipelineLayouts)
		{
			PipelineLayout->GetDescriptorPoolManager()->ResetPools();
		}

		g_VulkanRenderSystem->GetFrameCompletedDelegate().Broadcast();
	}

	ImagesInFlight[CurrentImageIndex] = *InFlightFences[CurrentFrame];
}

void CVulkanSwapChain::Present(CVulkanQueue* InPresentQueue)
{
	vk::PresentInfoKHR PresentInfo(
		1,
		&*RenderFinishedSemaphores[CurrentFrame],
		1,
		&*SwapChain,
		&CurrentImageIndex);

	vk::Result Result = InPresentQueue->GetQueue().presentKHR(PresentInfo);

	if (Result == vk::Result::eErrorOutOfDateKHR ||
		Result == vk::Result::eSuboptimalKHR || 
		bShouldRecreate)
	{
		/** Swap chain is out of date, recreate it and broadcast the message */
		OnSwapChainOutOfDate();
	}

	CurrentFrame = (CurrentFrame + 1) % g_MaxFramesInFlight;
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
	vk::SurfaceCapabilitiesKHR& InCapabilities) const
{
	if (InCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return InCapabilities.currentExtent;
	else
	{
		/** Default set current window width & height */
		vk::Extent2D Extent(
			g_Engine->GetWindow()->GetWidth(),
			g_Engine->GetWindow()->GetHeight());

		/** Clamp to swap chain capabilities */
		Extent.width = std::clamp(Extent.width, InCapabilities.minImageExtent.width,
			InCapabilities.maxImageExtent.width);
		Extent.height = std::clamp(Extent.height, InCapabilities.minImageExtent.height,
			InCapabilities.maxImageExtent.height);

		return Extent;
	}
}