#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystem.h"

class CVulkanDevice;

/**
 * Vulkan render system
 */
class CVulkanRenderSystem : public IRenderSystem
{
public:
	void Initialize() override;
	void Destroy() override;
	void NewFrame() override;
	void WaitGPU() override;

	TOwnerPtr<CRSSurface> CreateSurface(const SRSSurfaceCreateInfo& InCreateInfo) const override;

	TOwnerPtr<CRSBuffer> CreateBuffer(const SRSBufferCreateInfo& InCreateInfo) const override;

	TOwnerPtr<CRSTexture> CreateTexture(const SRSTextureCreateInfo& InCreateInfo) const override;

	TOwnerPtr<CRSSampler> CreateSampler(const SRSSamplerCreateInfo& InCreateInfo) const override;

	TOwnerPtr<CRSShader> CreateShader(const SRSShaderCreateInfo& InCreateInfo) const override;

	/** Utils */
	const char* GetName() const override { return "Vulkan"; }

	const vk::Instance& GetInstance() const { return *Instance; }
	CVulkanDevice* GetDevice() const { return Device.get(); }
private:
	std::vector<const char*> GetRequiredExtensions() const;
	bool IsDeviceUseable(const vk::PhysicalDevice& InDevice) const;
private:
	vk::UniqueInstance Instance;
	vk::DebugUtilsMessengerEXT Callback;
	std::unique_ptr<CVulkanDevice> Device;
};