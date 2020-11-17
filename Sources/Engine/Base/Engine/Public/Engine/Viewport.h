#pragma once

#include "EngineCore.h"
#include "Gfx/Resource.h"
#include "Gfx/Backend.h"

namespace ze
{

/**
 * A viewport on a window, draws to an color attachment
 */
class ENGINE_API Viewport
{
public:
    Viewport(float in_x,
        float in_y,
        float in_width,
        float in_height,
        const gfx::SharedTextureView& in_color_attachment_view = gfx::SharedTextureView());

    void resize(const float in_width, const float in_height);

    ZE_FORCEINLINE const gfx::ResourceHandle& get_texture() const { return *color_attachment; }
    ZE_FORCEINLINE const gfx::ResourceHandle& get_color_attachment_view() const { return *color_attachment_view; }
    ZE_FORCEINLINE gfx::Viewport get_viewport() const { return viewport; }
private:
    gfx::UniqueTexture color_attachment;
    gfx::SharedTextureView color_attachment_view;
    gfx::Viewport viewport;
};

} /* namespace ZE */