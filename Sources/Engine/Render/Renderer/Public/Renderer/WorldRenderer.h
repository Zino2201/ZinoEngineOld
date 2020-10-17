#pragma once

#include "EngineCore.h"
#include "WorldView.h"
#include "RendererTransientData.h"

namespace ze::renderer
{

class CClusteredForwardRenderingPath;
class CRenderPassDrawcallFactory;

/**
 * Per view object that manage the whole world rendering
 */
class CWorldRenderer 
{
public:
    CWorldRenderer(const SWorldView& InView);
    ~CWorldRenderer();

    void CheckVisibility();

    /**
     * Copy game state to render transient data
     * Once finished, it is safe to modify game data
     */
    void CopyGameState(TransientPerFrameDataMap& InPerFrameMap);

    /**
     * Prepare the renderer data for rendering
     */
    void Prepare();

    /**
     * Draw the world
     */
    void Draw();
private:
private:
    std::unique_ptr<CClusteredForwardRenderingPath> RenderingPath;
    std::vector<CRenderableComponentProxy*> VisibleProxies;
    std::vector<STransientProxyDataPerView> PerViewData;
    SWorldView View;
    std::vector<std::unique_ptr<CRenderPassDrawcallFactory>> Factories;
};

}