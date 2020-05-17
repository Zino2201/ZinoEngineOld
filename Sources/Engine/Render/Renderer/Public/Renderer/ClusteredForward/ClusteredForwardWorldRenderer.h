#pragma once

#include "Renderer/WorldRenderer.h"

namespace ZE::Renderer
{

/**
 * The main clustered forward hybrid deferred renderer
 */
class RENDERER_API CClusteredForwardWorldRenderer final : public IWorldRenderer
{
public:
    CClusteredForwardWorldRenderer() = default;

    void Render(CWorldProxy* InWorld, const SWorldRendererView& InView) override;
private:
    void RenderWorld(CWorldProxy* InWorld);
};

}