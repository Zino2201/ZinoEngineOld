#pragma once

#include "Render/RenderSystem/RenderSystem.h"
#include "VulkanCore.h"

struct SDL_Window;
class CVulkanDevice;
class CVulkanSwapChain;
class CVulkanRenderCommandContext;
class CVulkanCommandPool;
class CVulkanTexture;
class CVulkanTextureView;

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
	virtual CRenderSystemShaderPtr CreateShader(void* InData,
		size_t InDataSize,
		const EShaderStage& InShaderStage) override;
	virtual IRenderSystemGraphicsPipelinePtr CreateGraphicsPipeline(const SRenderSystemGraphicsPipelineInfos& InInfos) override;
	virtual CRenderSystemBufferPtr CreateBuffer(const SRenderSystemBufferInfos& InInfos) override;
	virtual IRenderSystemVertexBufferPtr CreateVertexBuffer(const uint64_t& InSize,
		EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
		bool bUsePersistentMapping = false,
		const std::string& InDebugName = "VertexBuffer") override;
	virtual IRenderSystemIndexBufferPtr CreateIndexBuffer(const uint64_t& InSize,
		EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
		bool bUsePersistentMapping = false,
		const std::string& InDebugName = "IndexBuffer") override;
	virtual IRenderSystemUniformBufferPtr CreateUniformBuffer(const SRenderSystemUniformBufferInfos& InInfos) override;
	virtual CRenderSystemTexturePtr CreateTexture(const SRenderSystemTextureInfo& InInfos) override;
	virtual CRenderSystemTextureViewPtr CreateTextureView(const SRenderSystemTextureViewInfo& InInfos) override;
	virtual CRenderSystemSamplerPtr CreateSampler(const SRenderSystemSamplerInfo& InInfos) override;
	virtual SRenderSystemDetails GetRenderSystemDetails() const;
	virtual TMulticastDelegate<>& GetFrameCompletedDelegate() override { return FrameCompletedDelegate; }

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

	vk::Format FindSupportedFormat(const vk::PhysicalDevice& InDevice,
		const std::vector<vk::Format>& InCandidates,
		vk::ImageTiling InTiling, vk::FormatFeatureFlags InFeatures) const;

	vk::Format FindDepthFormat(const vk::PhysicalDevice& InDevice) const;
	bool HasStencilComponent(vk::Format InFormat) const;
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

	/** Depth buffer */
	boost::intrusive_ptr<CVulkanTexture> DepthBuffer;

	/** Depth buffer view */
	boost::intrusive_ptr<CVulkanTextureView> DepthBufferView;

	/** Render pass */
	vk::UniqueRenderPass RenderPass;

	/** Swap chain framebuffers */
	std::vector<vk::UniqueFramebuffer> Framebuffers;

	/** Render command context */
	std::unique_ptr<CVulkanRenderCommandContext> RenderCommandContext;

	/** Memory transfer operations command pool */
	std::unique_ptr<CVulkanCommandPool> MemoryPool;

	TMulticastDelegate<> FrameCompletedDelegate;

};