#include "Render/VulkanRenderSystem/VulkanSurface.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "VulkanRenderSystemContext.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

CVulkanSurface::CVulkanSurface(CVulkanDevice* InDevice, 
	void* InWindowHandle, const uint32_t& InWidth, const uint32_t& InHeight,
	const SRSResourceCreateInfo& InInfo)
	: CRSSurface(InWindowHandle, InWidth, InHeight, InInfo), 
	CVulkanDeviceResource(InDevice), WindowHandle(InWindowHandle), bHasBeenResized(false)
{
	/**
	 * Create vulkan surface
	 */
	{
		VkSurfaceKHR SurfaceTmp;

		if (!SDL_Vulkan_CreateSurface(reinterpret_cast<SDL_Window*>(InWindowHandle),
			static_cast<VkInstance>(GVulkanRenderSystem->GetInstance()),
			&SurfaceTmp))
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create surface: %s",
				 SDL_GetError());

		Surface = vk::UniqueSurfaceKHR(SurfaceTmp, vk::ObjectDestroy<vk::Instance,
			VULKAN_HPP_DEFAULT_DISPATCHER_TYPE>(GVulkanRenderSystem->GetInstance(),
				nullptr, vk::DispatchLoaderStatic()));

		/** Create present queue if not created */
		Device->CreatePresentQueue(*Surface);
	}

	/**
	 * Create swapchain
	 */
	{
		SwapChain = std::make_unique<CVulkanSwapChain>(Device,
			InWidth, 
			InHeight, 
			this);
	}
}

bool CVulkanSurface::AcquireImage()
{
	vk::Result Result = SwapChain->AcquireImage();

	/** If swap chain if out of date or suboptimal, recreate it */
	if (Result == vk::Result::eErrorOutOfDateKHR ||
		Result == vk::Result::eSuboptimalKHR)
	{
		RecreateSwapChain();
		return true;
	}
	else if(Result != vk::Result::eSuccess)
	{
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to acquire swap chain image");
	}

	return true;
}

void CVulkanSurface::Present(CVulkanQueue* InPresentQueue)
{
	SwapChain->Present(InPresentQueue);
}

void CVulkanSurface::RecreateSwapChain()
{
	LOG(ELogSeverity::Debug, VulkanRS, "Recreated swap chain");

	/**
	 * Submit all commands and wait
	 */
	Device->WaitIdle();

	/**
	 * Acquire old swap chain handle to give to new swap chain
	 */
	vk::SwapchainKHR Handle = SwapChain->AcquireHandle();
	SwapChain = std::make_unique<CVulkanSwapChain>(Device,
		Width,
		Height,
		this,
		Handle);
	Device->GetDevice().destroySwapchainKHR(Handle);

	/**
	 * Clear all resources
	 */
	Device->GetDeferredDestructionMgr().DestroyResources();
}

void CVulkanSurface::Resize(const uint32_t& InWidth, const uint32_t& InHeight)
{
	Width = InWidth;
	Height = InHeight;

	RecreateSwapChain();
}

EFormat CVulkanSurface::GetSwapChainFormat() const
{
	return VulkanUtil::VkFormatToFormat(SwapChain->GetSurfaceFormat().format);
}

CRSTexture* CVulkanSurface::GetBackbufferTexture()
{ 
	return SwapChain->GetBackbufferTexture();
}

CRSSurface* CVulkanRenderSystem::CreateSurface(void* InWindowHandle,
	const uint32_t& InWidth, const uint32_t& InHeight,
	const SRSResourceCreateInfo& InInfo) const
{
	return new CVulkanSurface(
		Device.get(),
		InWindowHandle,
		InWidth,
		InHeight,
		InInfo);
}