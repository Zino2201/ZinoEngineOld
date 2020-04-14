#pragma once

#include "VulkanCore.h"

class CVulkanSwapChain;

class CVulkanSurface : public CRSSurface,
	public CVulkanDeviceResource
{
public:
	CVulkanSurface(CVulkanDevice* InDevice,
		void* InWindowHandle,
		const uint32_t& InWidth, const uint32_t& InHeight,
		const SRSResourceCreateInfo& InInfo);

	CVulkanSwapChain* GetSwapChain() const { return SwapChain.get(); }

	virtual EFormat GetSwapChainFormat() const override;
	virtual CRSTexture* GetBackbufferTexture() const override;
protected:
	vk::UniqueSurfaceKHR Surface;
	std::unique_ptr<CVulkanSwapChain> SwapChain;
};