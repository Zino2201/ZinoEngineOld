#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ze
{

/**
 * Represents a viewport
 */
class ENGINE_API CViewport
{
public:
    CViewport(void* InWindowHandle, const uint32_t& InWidth,
        const uint32_t& InHeight, const bool& bInVSync);

    bool Begin();
    void End();
    void Resize(const uint32_t& InWidth, const uint32_t& InHeight);
    void SetVSync(const bool& bInVSync);

    CRSSurface* GetSurface() const { return Surface.get(); }
private:
    CRSSurfacePtr Surface;
    void* WindowHandle;
    uint32_t Width;
    uint32_t Height;
};

} /* namespace ZE */