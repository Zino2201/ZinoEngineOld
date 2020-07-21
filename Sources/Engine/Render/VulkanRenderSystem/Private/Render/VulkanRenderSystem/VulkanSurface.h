#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/Resources/Surface.h"

class CVulkanSwapChain;
class CVulkanQueue;

class CVulkanSurface : public CRSSurface,
	public CVulkanDeviceResource
{
public:
	CVulkanSurface(CVulkanDevice& InDevice,
		const SRSSurfaceCreateInfo& InCreateInfo);

	/**
	 * Resize the viewport
	 * Recreate the swap chain
	 */
	void Resize(const uint32_t& InWidth, const uint32_t& InHeight) override;

	void ResetOldSwapchain();

	/**
	 * Try acquire image
	 * If swap chain is recreated, return false and the frame MUST be skipped
	 */
	bool AcquireImage();
	void Present(CVulkanQueue* InPresentQueue);

	EFormat GetSwapChainFormat() const override;
	CRSTexture* GetBackbufferTexture() override;
	const vk::SurfaceKHR& GetSurface() const { return Surface.get(); }
	CVulkanSwapChain* GetSwapChain() const { return SwapChain.get(); }
	void* GetWindowHandle() const { return WindowHandle; }
private:
	void RecreateSwapChain();
protected:
	vk::UniqueSurfaceKHR Surface;
	std::unique_ptr<CVulkanSwapChain> SwapChain;
	std::unique_ptr<CVulkanSwapChain> OldSwapChain;
	void* WindowHandle;
	bool bHasBeenResized;
	bool bUseVSync;
};