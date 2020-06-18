#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include <array>
#include <vector>

namespace ZE
{

/**
 * Render system context
 */
class IRenderSystemContext
{
public:
    virtual ~IRenderSystemContext() = default;

	virtual void BeginRenderPass(const SRSRenderPass& InRenderPass,
		const SRSFramebuffer& InFrameBuffer,
        const std::array<float, 4>& InClearColor = { 0, 0, 0, 1 },
        const char* InName = "Unnamed Render Pass") = 0;

    virtual void EndRenderPass() = 0;

    /** Surfaces */
    virtual bool BeginSurface(CRSSurface* InSurface) = 0;
    virtual void PresentSurface(CRSSurface* InSurface) = 0;

    /** States commands */
    virtual void BindGraphicsPipeline(CRSGraphicsPipeline* InGraphicsPipeline) = 0;
    virtual void BindGraphicsPipeline(const SRSGraphicsPipeline& InGraphicsPipeline) = 0;
    virtual void SetViewports(const std::vector<SViewport>& InViewports) = 0;
    virtual void SetScissors(const std::vector<ZE::Math::SRect2D>& InScissors) = 0;
    
    /** Resources bindings */
    virtual void BindVertexBuffers(const std::vector<CRSBuffer*> InVertexBuffers) = 0;
    virtual void BindIndexBuffer(CRSBuffer* InIndexBuffer, 
        const uint64_t& InOffset,
        const EIndexFormat& InIndexFormat) = 0;
	virtual void SetShaderUniformBuffer(const uint32_t& InSet, const uint32_t& InBinding,
		CRSBuffer* InBuffer) = 0;
	virtual void SetShaderTexture(const uint32_t& InSet, const uint32_t& InBinding,
		CRSTexture* InTexture) = 0;
	virtual void SetShaderSampler(const uint32_t& InSet, const uint32_t& InBinding,
		CRSSampler* InSampler) = 0;

    /** Draw commands */
	virtual void Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertex, const uint32_t& InFirstInstance) = 0;
	virtual void DrawIndexed(const uint32_t& InIndexCount,
		const uint32_t& InInstanceCount, const uint32_t& InFirstIndex,
		const int32_t& InVertexOffset, const uint32_t& InFirstInstance) = 0;
};

}