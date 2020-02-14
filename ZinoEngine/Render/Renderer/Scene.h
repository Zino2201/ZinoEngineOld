#pragma once

#include "Render/RenderCore.h"

class CWorld;
class CRenderableComponent;

/**
 * Represents a scene
 * Each must world has one to be properly rendered
 */
class CScene
{
public:
    CScene(CWorld* InWorld);

    void AddRenderable(CRenderableComponent* InComponent);
    void DeleteRenderable(CRenderableComponent* InComponent);

    const std::map<CRenderableComponent*, CRenderableComponentProxy*>& GetRenderableComponents() const { return Proxies; }
private:
    void AddRenderable_RenderThread(CRenderableComponent* InComponent,
        CRenderableComponentProxy* InProxy);
	void DeleteRenderable_RenderThread(CRenderableComponent* InComponent);
private:
    CWorld* World;
    std::map<CRenderableComponent*, CRenderableComponentProxy*> Proxies;
};