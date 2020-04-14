#include "Render/VulkanRenderSystem/VulkanSurface.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

CVulkanSurface::CVulkanSurface(CVulkanDevice* InDevice, 
	void* InWindowHandle, const uint32_t& InWidth, const uint32_t& InHeight,
	const SRSResourceCreateInfo& InInfo)
	: CRSSurface(InWindowHandle, InWidth, InHeight, InInfo), CVulkanDeviceResource(InDevice)
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
			InWidth, InHeight, *Surface);
	}
}

EFormat CVulkanSurface::GetSwapChainFormat() const
{
	return VulkanUtil::VkFormatToFormat(SwapChain->GetSurfaceFormat().format);
}

CRSTexture* CVulkanSurface::GetBackbufferTexture() const
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