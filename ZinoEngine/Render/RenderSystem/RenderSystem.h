#pragma once

#include "Render/RenderCore.h"
#include "RenderSystemResources.h"

class IRenderCommandContext;
class CRenderSystemShader;
class IRenderSystemGraphicsPipeline;

/**
 * Render system interface
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	/**
	 * Initialize render system
	 */
	virtual void Initialize() = 0;

	/**
	 * Prepare frame
	 */
	virtual void Prepare() = 0;

	/**
	 * Present image
	 */
	virtual void Present() = 0;

	/**
	 * Prepare destroy (wait until GPU complete all operations before deleting objects)
	 */
	virtual void WaitGPU() = 0;

	/**
	 * Get command context
	 */
	virtual IRenderCommandContext* GetRenderCommandContext() const = 0;

	/**
	 * Create a shader
	 */
	virtual CRenderSystemShaderPtr CreateShader(void* InData,
		size_t InDataSize,
		const EShaderStage& InShaderStage) = 0;

	/**
	 * Create a buffer
	 */
	virtual CRenderSystemBufferPtr CreateBuffer(const SRenderSystemBufferInfos& InInfos) = 0;

	/**
	 * Create a vertex buffer
	 */
	virtual IRenderSystemVertexBufferPtr CreateVertexBuffer(const uint64_t& InSize,
		EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
		bool bUsePersistentMapping = false,
		const std::string& InDebugName = "VertexBuffer") = 0;
	
	/**
	 * Create a index buffer
	 */
	virtual IRenderSystemIndexBufferPtr CreateIndexBuffer(const uint64_t& InSize,
		EBufferMemoryUsage InMemoryUsage = EBufferMemoryUsage::GpuOnly,
		bool bUsePersistentMapping = false,
		const std::string& InDebugName = "IndexBuffer") = 0;

	/**
	 * Create a graphics pipeline
	 */
	virtual IRenderSystemGraphicsPipelinePtr CreateGraphicsPipeline(const SRenderSystemGraphicsPipelineInfos& InInfos) = 0;

	/** 
	 * Create a uniform buffer 
	 */
	virtual IRenderSystemUniformBufferPtr CreateUniformBuffer(const SRenderSystemUniformBufferInfos& InInfos) = 0;

	/**
	 * Create a texture
	 */
	virtual CRenderSystemTexturePtr CreateTexture(const SRenderSystemTextureInfo& InInfos) = 0;

	/**
	 * Create a texture view
	 */
	virtual CRenderSystemTextureViewPtr CreateTextureView(const SRenderSystemTextureViewInfo& InInfos) = 0;

	/*
	 * Create a sampler
	 */
	virtual CRenderSystemSamplerPtr CreateSampler(const SRenderSystemSamplerInfo& InInfos) = 0;

	virtual SRenderSystemDetails GetRenderSystemDetails() const = 0;

	virtual TMulticastDelegate<>& GetFrameCompletedDelegate() = 0;
};