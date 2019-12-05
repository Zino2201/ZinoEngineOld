#pragma once

#include "RenderCore.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "TextureView.h"
#include "Sampler.h"

class IRenderCommandContext;
class IShader;
class IGraphicsPipeline;

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
	virtual std::shared_ptr<IShader> CreateShader(const std::vector<uint8_t>& InData,
		const EShaderStage& InShaderStage) = 0;

	/**
	 * Create a buffer
	 */
	virtual std::shared_ptr<IBuffer> CreateBuffer(const SBufferInfos& InInfos) = 0;

	/**
	 * Create a graphics pipeline
	 */
	virtual std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(const SGraphicsPipelineInfos& InInfos) = 0;

	/** 
	 * Create a uniform buffer 
	 */
	virtual std::shared_ptr<IUniformBuffer> CreateUniformBuffer(const SUniformBufferInfos& InInfos) = 0;

	/**
	 * Create a texture
	 */
	virtual std::shared_ptr<ITexture> CreateTexture(const STextureInfo& InInfos) = 0;

	/**
	 * Create a texture view
	 */
	virtual std::shared_ptr<ITextureView> CreateTextureView(const STextureViewInfo& InInfos) = 0;

	/*
	 * Create a sampler
	 */
	virtual std::shared_ptr<ISampler> CreateSampler(const SSamplerInfo& InInfos) = 0;
};