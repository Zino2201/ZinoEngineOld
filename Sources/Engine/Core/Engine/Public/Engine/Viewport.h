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

	void InitResource_RenderThread() override;
	void DestroyResource_RenderThread() override;

    /** Render-thread only */
    bool Begin();
    void End();
    void Resize(const uint32_t& InWidth, const uint32_t& InHeight);

    CRSSurface* GetSurface() const { return Surface.get(); }
private:
    CRSSurfacePtr Surface;
    void* WindowHandle;
    uint32_t Width;
    uint32_t Height;
};

} /* namespace ZE */