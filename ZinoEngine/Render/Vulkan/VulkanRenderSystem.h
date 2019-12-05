#pragma once

#include "Render/RenderSystem.h"
#include "VulkanCore.h"
#include "Render/Texture.h"

struct SDL_Window;
class CVulkanDevice;
class CVulkanSwapChain;
class CVulkanRenderCommandContext;
class CVulkanCommandPool;

/**
 * Vulkan render system
 */
class CVulkanRenderSystem : public IRenderSystem
{
public:
	CVulkanRenderSystem();
	virtual ~CVulkanRenderSystem();

	/** IRenderSystem */
	virtual void Prepare() override;
	virtual void Initialize() override;
	void AcquireImage();
	virtual void Present() override;
	virtual void WaitGPU() override;
	virtual std::shared_ptr<IShader> CreateShader(const std::vector<uint8_t>& InData,
		const EShaderStage& InShaderStage) override;
	virtual std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(const SGraphicsPipelineInfos& InInfos) override;
	virtual std::shared_ptr<IBuffer> CreateBuffer(const SBufferInfos& InInfos) override;
	virtual std::shared_ptr<IUniformBuffer> CreateUniformBuffer(const SUniformBufferInfos& InInfos) override;
	virtual std::shared_ptr<ITexture> CreateTexture(const STextureInfo& InInfos) override;
	virtual std::shared_ptr<ITextureView> CreateTextureView(const STextureViewInfo& InInfos) override;
	virtual std::shared_ptr<ISampler> CreateSampler(const SSamplerInfo& InInfos) override;

	/** Getters */
	const vk::Instance& GetInstance() const { return *Instance; }
	const vk::SurfaceKHR& GetSurface() const { return *Surface; }
	const vk::RenderPass& GetRenderPass() const { return *RenderPass; }
	const std::vector<vk::UniqueFramebuffer>& GetFramebuffers() { return Framebuffers; }
	CVulkanDevice* GetDevice() const { return Device.get(); }
	CVulkanSwapChain* GetSwapChain() const { return SwapChain.get(); }
	CVulkanCommandPool* GetMemoryPool() const { return MemoryPool.get(); }
	IRenderCommandContext* GetRenderCommandContext() const override 
	{ 
		return reinterpret_cast<IRenderCommandContext*>(RenderCommandContext.get()); 
	}
private:
	void OnSwapchainRecreated();
	void CreateSwapchainObjects();

	/**
	 * Get required Vulkan extensions
	 */
	std::vector<const char*> GetRequiredExtensions(SDL_Window* InWindow) const;

	/**
	 * Check if required layers are supported
	 */
	bool IsRequiredLayersSupported() const;

	/**
	 * Check if physical device is useable
	 */
	bool IsDeviceUseable(const vk::PhysicalDevice& InDevice) const;
private:
	/** Vulkan instance */
	vk::UniqueInstance Instance;

	/** Surface */
	vk::UniqueSurfaceKHR Surface;

	/** Debug callback */
	vk::DebugUtilsMessengerEXT Callback;

	/** Current device */
	std::unique_ptr<CVulkanDevice> Device;

	/** Swap chain */
	std::unique_ptr<CVulkanSwapChain> SwapChain;

	/** Render pass */
	vk::UniqueRenderPass RenderPass;

	/** Swap chain framebuffers */
	std::vector<vk::UniqueFramebuffer> Framebuffers;

	/** Render command context */
	std::unique_ptr<CVulkanRenderCommandContext> RenderCommandContext;

	/** Memory transfer operations command pool */
	std::unique_ptr<CVulkanCommandPool> MemoryPool;
};