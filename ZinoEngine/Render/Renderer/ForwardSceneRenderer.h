#pragma once

#include "SceneRenderer.h"

/**
 * Simple forward renderer
 * Inefficient with multiple lights
 */
class CForwardSceneRenderer : public ISceneRenderer
{
public:
    virtual void Render(IRenderCommandContext* InCommandContext,
        CScene* InScene) override;
private:
    void DrawWorld(IRenderCommandContext* InCommandContext,
        CScene* InScene);
};