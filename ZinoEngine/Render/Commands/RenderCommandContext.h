#pragma once

#include "Render/RenderSystem/RenderSystemResources.h"

/**
 * Render command context
 *
 * Used for rendering
 */
class IRenderCommandContext
{
public:
	virtual ~IRenderCommandContext() = default;

	/**
	 * Begin command recording 
	 */
	virtual void Begin() = 0;

	/** 
	 * End command recording 
	 */
	virtual void End() = 0;

	/**
	 * Begin render pass
	 */
	virtual void BeginRenderPass(const std::array<float, 4>& InClearColor,
		const std::string& InName = "RenderPass") = 0;

	/**
	 * End render pass
	 */
	virtual void EndRenderPass() = 0;

	/**
	 * Bind graphics pipeline
	 */
	virtual void BindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline) = 0;

	/**
	 * Bind vertex buffers
	 */
	virtual void BindVertexBuffers(const std::vector<IRenderSystemVertexBuffer*>& InVertexBuffers) = 0;

	/**
	 * Bind index buffer
	 */
	virtual void BindIndexBuffer(IRenderSystemIndexBuffer* InIndexBuffer,
		const uint64_t& InOffset,
		const EIndexFormat& InIndexFormat) = 0;

	/** Shader parameter bindings */

	/**
	 * Set a uniform buffer
	 */
	virtual void SetShaderUniformBuffer(const uint32_t& InSet,
		const uint32_t& InBinding, IRenderSystemUniformBuffer* InUBO) = 0;

	/**
	 * Set a combined image sampler
	 */
	virtual void SetShaderCombinedImageSampler(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemTextureView* InView) = 0;
	
	virtual void SetShaderStorageBuffer(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemBuffer* InBuffer) = 0;

	/**
	 * Draw
	 */
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) = 0;

	virtual void DrawIndexed(const uint32_t& InIndexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstIndex, const int32_t& InVertexOffset, const uint32_t& InFirstInstance) = 0;

	/** 
	 * Render state
	 */
	virtual void SetViewports(const std::vector<SViewport>& InViewports) = 0;
	virtual void SetScissors(const std::vector<SRect2D>& InScissors) = 0;
};