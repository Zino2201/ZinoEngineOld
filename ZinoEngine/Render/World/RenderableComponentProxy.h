#pragma once

#include "Core/EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

/**
 * Proxies used by the render thread to draw the scene and other
 */

/**
 * Renderable component proxy base class
 */
class CRenderableComponentProxy
{
public:
    CRenderableComponentProxy(const CRenderableComponent* InComponent);
    virtual ~CRenderableComponentProxy() {}

    virtual void Draw(IRenderCommandContext* InCommandContext);
protected:
    STransform Transform;
    IRenderSystemUniformBufferPtr UniformBuffer;
};