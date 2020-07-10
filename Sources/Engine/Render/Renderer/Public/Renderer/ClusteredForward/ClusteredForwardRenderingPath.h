#pragma once

#include "Renderer/RenderingPath.h"

namespace ZE { class IRenderSystemContext; }
namespace ZE::UI { class CImGuiRender; }

namespace ZE::Renderer
{

/**
 * The main clustered forward hybrid deferred renderer
 */
class RENDERER_API CClusteredForwardRenderingPath final : public IRenderingPath
{
public:
    CClusteredForwardRenderingPath();

    void Draw(const SWorldView& InView) override;
private:
   /* void RenderWorld(IRenderSystemContext* InContext, CWorldProxy* InWorld, 
        const SWorldRendererView& InView,
        EMeshRenderPass InRenderPass);*/
};

}