#pragma once

#include "Renderer.h"

/**
 * Simple forward renderer
 * Inefficient with multiple lights
 */
class CForwardRenderer : public IRenderer
{
public:
    virtual void Render(CRenderCommandList* InCommandList) override;
private:
    void DrawWorld(CRenderCommandList* InCommandList);
};