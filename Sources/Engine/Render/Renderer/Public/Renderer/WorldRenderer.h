#pragma once

#include "EngineCore.h"

namespace ZE { class CRSSurface; }

namespace ZE::Renderer
{

class CWorldProxy;

/**
 * Informations about the view
 */
struct SWorldRendererView
{
    boost::intrusive_ptr<CRSSurface> Surface;
    uint32_t Width;
    uint32_t Height;
    Math::STransform Position;
    float FOV;
    float NearPlane;
    float FarPlane;
};

/**
 * Base interface for a world renderer
 */
class RENDERER_API IWorldRenderer
{
public:
    /**
     * Render a world
     */
    virtual void Render(CWorldProxy* InWorld, const SWorldRendererView& InView) = 0;
};

}