#pragma once

#include "EngineCore.h"

namespace ZE::Renderer
{

class CRenderableComponentProxy;

/**
 * Render-thread version of the world
 */
class CWorldProxy final
{
public:
    RENDERER_API CWorldProxy();
    RENDERER_API ~CWorldProxy();

    RENDERER_API void AddComponent(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
    RENDERER_API void RemoveComponent(CRenderableComponentProxy* InProxy);
private:
    void AddComponent_RenderThread(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
    void RemoveComponent_RenderThread(CRenderableComponentProxy* InProxy);
private:
    std::vector<std::unique_ptr<CRenderableComponentProxy>> Proxies;
};

}