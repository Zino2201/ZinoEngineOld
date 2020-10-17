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

	OwnerPtr<CRSSurface> CreateSurface(const SRSSurfaceCreateInfo& InCreateInfo) const override;
	OwnerPtr<CRSBuffer> CreateBuffer(const SRSBufferCreateInfo& InCreateInfo) const override;
	OwnerPtr<CRSTexture> CreateTexture(const SRSTextureCreateInfo& InCreateInfo) const override;
	OwnerPtr<CRSSampler> CreateSampler(const SRSSamplerCreateInfo& InCreateInfo) const override;
	OwnerPtr<gfx::shaders::CRSShader> CreateShader(const gfx::shaders::SRSShaderCreateInfo& InCreateInfo) const override;

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