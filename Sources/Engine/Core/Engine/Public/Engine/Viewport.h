#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE
{

DECLARE_LOG_CATEGORY(Viewport);

/**
 * Represents a viewport
 */
class CViewport
{
public:
    CViewport(void* InWindowHandle, const uint32_t& InWidth,
        const uint32_t& InHeight);

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