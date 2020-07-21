#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE
{

class IRenderCommandContext;
class SShaderParameterMap;

RENDERSYSTEM_API extern class IRenderSystem* GRenderSystem;
RENDERSYSTEM_API extern class IRenderSystemContext* GRSContext;

/**
 * Render system interface
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
    virtual void NewFrame() = 0;
    virtual void WaitGPU() = 0;

	virtual TOwnerPtr<CRSSurface> CreateSurface(const SRSSurfaceCreateInfo& InCreateInfo) const = 0;
	virtual TOwnerPtr<CRSBuffer> CreateBuffer(const SRSBufferCreateInfo& InCreateInfo) const = 0;
	virtual TOwnerPtr<CRSTexture> CreateTexture(const SRSTextureCreateInfo& InCreateInfo) const = 0;
	virtual TOwnerPtr<CRSSampler> CreateSampler(const SRSSamplerCreateInfo& InCreateInfo) const = 0;
	virtual TOwnerPtr<CRSShader> CreateShader(const SRSShaderCreateInfo& InCreateInfo) const = 0;

	/** Utils */
	virtual const char* GetName() const = 0;
};

}