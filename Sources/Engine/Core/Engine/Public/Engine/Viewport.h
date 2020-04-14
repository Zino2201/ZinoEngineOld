#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderThreadResource.h"

namespace ZE
{

DECLARE_LOG_CATEGORY(Viewport);

/**
 * Represents a viewport
 */
class CViewport : public CRenderThreadResource
{
public:
    CViewport(void* InWindowHandle, const uint32_t& InWidth,
        const uint32_t& InHeight);

	virtual void InitResource_RenderThread() override;
	virtual void DestroyResource_RenderThread() override;

    /** Render-thread only */
    void Begin();
    void End();

    CRSSurface* GetSurface() const { return Surface.get(); }
    CRSTexture* GetDepthBuffer() const { return DepthBuffer.get(); }
private:
    CRSSurfacePtr Surface;
    void* WindowHandle;
    uint32_t Width;
    uint32_t Height;
    CRSTexturePtr DepthBuffer;
};

} /* namespace ZE */