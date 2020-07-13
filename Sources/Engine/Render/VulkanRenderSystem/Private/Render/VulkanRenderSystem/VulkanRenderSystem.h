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
	virtual void Initialize() override;
	virtual void Destroy() override;
	void NewFrame() override;
	virtual void WaitGPU() override;

	virtual CRSSurface* CreateSurface(void* InWindowHandle,
		const uint32_t& InWidth, const uint32_t& InHeight,
		const bool& bInUseVSync,
		const SRSResourceCreateInfo& InInfo = {}) const override;

	virtual CRSBuffer* CreateBuffer(
		const ERSBufferUsage& InUsageFlags,
		const ERSMemoryUsage& InMemUsage,
		const uint64_t& InSize,
		const SRSResourceCreateInfo& InInfo) const override;

	virtual CRSTexture* CreateTexture(
		const ERSTextureType& InTextureType,
		const ERSTextureUsage& InTextureUsage,
		const ERSMemoryUsage& InMemoryUsage,
		const EFormat& InFormat,
		const uint32_t& InWidth,
		const uint32_t& InHeight,
		const uint32_t& InDepth,
		const uint32_t& InArraySize,
		const uint32_t& InMipLevels,
		const ESampleCount& InSampleCount,
		const SRSResourceCreateInfo& InInfo) const override;

	CRSSampler* CreateSampler(const SRSSamplerCreateInfo& InCreateInfo) const override;

	virtual CRSShader* CreateShader(
		const EShaderStage& InStage,
		const uint64_t& InBytecodeSize,
		const void* InBytecode,
		const SShaderParameterMap& InParameterMap, 
		const SRSResourceCreateInfo& InCreateInfo = {}) const override;

	virtual CRSGraphicsPipeline* CreateGraphicsPipeline(
		const std::vector<SRSPipelineShaderStage>& InShaderStages,
		const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
		const SRSRenderPass& InRenderPass = SRSRenderPass(),
		const SRSBlendState& InBlendState = SRSBlendState(),
		const SRSRasterizerState& InRasterizerState = SRSRasterizerState(),
		const SRSDepthStencilState& InDepthStencilState = SRSDepthStencilState(),
		const SRSResourceCreateInfo& InCreateInfo = SRSResourceCreateInfo()) const override;

	/** Utils */
	virtual const char* GetName() const override { return "Vulkan"; }

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