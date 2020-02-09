#pragma once

#include "EngineCore.h"

class CRenderableComponent;
class CRenderCommandList;

/**
 * Contains proxies used by the render thread to draw the scene
 */
class CRenderableComponentProxy;

/**
 * Scene proxy
 */
class CSceneProxy
{
public:
    void AddRenderableComponentProxy(CRenderableComponentProxy* InProxy)
    {
        RenderableComponents.push_back(InProxy);
    }

    const std::vector<CRenderableComponentProxy*>& GetRenderableComponents() const { return RenderableComponents; }
private:
    std::vector<CRenderableComponentProxy*> RenderableComponents;
};

/**
 * Renderable component proxy
 */
class CRenderableComponentProxy
{
public:
    CRenderableComponentProxy(const CRenderableComponent* InComponent);

    virtual void Draw(CRenderCommandList* InCommandList);
protected:
    STransform Transform;
    bool bIsOutdated;
};