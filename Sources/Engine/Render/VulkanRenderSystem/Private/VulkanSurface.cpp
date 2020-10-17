#include "VulkanSurface.h"
#include "VulkanRenderSystem.h"
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystemContext.h"
#include <SDL.h>
#include <SDL_vulkan.h>

CVulkanSurface::CVulkanSurface(CVulkanDevice& InDevice, 
	const SRSSurfaceCreateInfo& InCreateInfo)
	: CRSSurface(InCreateInfo), 
	CVulkanDeviceResource(InDevice), WindowHandle(InCreateInfo.WindowHandle), bHasBeenResized(false),
	bUseVSync(InCreateInfo.bEnableVSync)
{
	/**
	 * Create vulkan surface
	 */
	{
		VkSurfaceKHR SurfaceTmp;

		if (!SDL_Vulkan_CreateSurface(reinterpret_cast<SDL_Window*>(InCreateInfo.WindowHandle),
			static_cast<VkInstance>(GVulkanRenderSystem->GetInstance()),
			&SurfaceTmp))
			ze::logger::fatal("Failed to create surface: {}",
				 SDL_GetError());

		Surface = vk::UniqueSurfaceKHR(SurfaceTmp, vk::ObjectDestroy<vk::Instance,
			VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(GVulkanRenderSystem->GetInstance(),
				nullptr, vk::DispatchLoaderStatic()));

		/** Create present queue if not created */
		Device.CreatePresentQueue(*Surface);
	}

	/**
	 * Create swapchain
	 */
	{
		SwapChain = std::make_unique<CVulkanSwapChain>(Device,
			InCreateInfo.Width, 
			InCreateInfo.Height,
			this,
			InCreateInfo.bEnableVSync);
	}
}

bool CVulkanSurface::AcquireImage()
{
	vk::Result Result = SwapChain->AcquireImage();

	/** If swap chain if out of date or suboptimal, recreate it */
	if (Result == vk::Result::eErrorOutOfDateKHR || bHasBeenResized)
	{
		RecreateSwapChain();
		return false;
	}
	else if(Result != vk::Result::eSuccess && Result != vk::Result::eSuboptimalKHR)
	{
		ze::logger::fatal("Failed to acquire swap chain image");
	}

	return true;
}

void CVulkanSurface::Present(CVulkanQueue* InPresentQueue)
{
	vk::Result Result = SwapChain->Present(InPresentQueue);

	/** If swap chain if out of date or suboptimal, recreate it */
	if (Result == vk::Result::eErrorOutOfDateKHR 
		|| Result == vk::Result::eSuboptimalKHR
		|| bHasBeenResized)
	{
		RecreateSwapChain();
	}
	else if(Result != vk::Result::eSuccess)
	{
		ze::logger::fatal("Failed to present swap chain image");
	}
}

void CVulkanSurface::ResetOldSwapchain() 
{ 
	OldSwapChain.reset(); 
}

void CVulkanSurface::RecreateSwapChain()
{
	/**
	 * Acquire old swap chain so we can still render a frame before removing it
	 */
	OldSwapChain.swap(SwapChain);
	SwapChain = std::make_unique<CVulkanSwapChain>(Device,
		CreateInfo.Width,
		CreateInfo.Height,
		this,
		CreateInfo.bEnableVSync,
		OldSwapChain->GetSwapChain());
	bHasBeenResized = false;
}

void CVulkanSurface::Resize(const uint32_t& InWidth, const uint32_t& InHeight)
{
	CreateInfo.Width = InWidth;
	CreateInfo.Height = InHeight;

	bHasBeenResized = true;
}

EFormat CVulkanSurface::GetSwapChainFormat() const
{
	return VulkanUtil::VkFormatToFormat(SwapChain->GetSurfaceFormat().format);
}

CRSTexture* CVulkanSurface::GetBackbufferTexture()
{ 
	return SwapChain->GetBackbufferTexture();
}

OwnerPtr<CRSSurface> CVulkanRenderSystem::CreateSurface(const SRSSurfaceCreateInfo& InCreateInfo) const
{
	return new CVulkanSurface(
		*Device.get(),
		InCreateInfo);
}