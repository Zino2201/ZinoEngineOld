#pragma once

#include "SceneRenderer.h"

/**
 * Simple forward renderer
 * Inefficient with multiple lights
 */
class CForwardSceneRenderer : public ISceneRenderer
{
public:
    virtual void Render(CRenderCommandList* InCommandList,
        CScene* InScene) override;
private:
    void DrawWorld(CRenderCommandList* InCommandList,
        CScene* InScene);
};