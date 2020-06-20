#pragma once

#include "Renderer/WorldRenderer.h"
#include "Renderer/MeshRenderPass.h"

namespace ZE { class IRenderSystemContext; }
namespace ZE::UI { class CImGuiRender; }

namespace ZE::Renderer
{

/**
 * The main clustered forward hybrid deferred renderer
 */
class RENDERER_API CClusteredForwardWorldRenderer final : public IWorldRenderer
{
public:
    CClusteredForwardWorldRenderer(ZE::UI::CImGuiRender& InImGuiRenderer);

    void Render(CWorldProxy* InWorld, const SWorldRendererView& InView) override;
private:
    void RenderWorld(IRenderSystemContext* InContext, CWorldProxy* InWorld, 
        const SWorldRendererView& InView,
        EMeshRenderPass InRenderPass);
private:
    ZE::UI::CImGuiRender& ImGuiRenderer;
};

}