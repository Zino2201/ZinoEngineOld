#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Gfx/Resource.h"

namespace ze
{

/**
 * A viewport on a window, draws to an render target
 */
class ENGINE_API Viewport
{
public:
    Viewport(void* in_parent_window,
        uint32_t in_width,
        uint32_t in_height,
        gfx::ResourceHandle in_render_target = gfx::ResourceHandle());

    void begin();
    void end();
private:
    void* parent_window;
    uint32_t width;
    uint32_t height;
    gfx::ResourceHandle render_target;
};

} /* namespace ZE */