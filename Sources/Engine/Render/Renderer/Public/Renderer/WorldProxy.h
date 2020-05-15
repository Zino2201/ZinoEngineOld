#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"

namespace ZE::Renderer
{

class CRenderableComponentProxy;

/**
 * Render-thread version of the world
 */
class RENDERER_API CWorldProxy final : public CNonCopyable
{
public:
    CWorldProxy();
    ~CWorldProxy();

    void AddComponent(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
    void RemoveComponent(CRenderableComponentProxy* InProxy);
private:
    void AddComponent_RenderThread(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
    void RemoveComponent_RenderThread(CRenderableComponentProxy* InProxy);
private:
    std::vector<std::unique_ptr<CRenderableComponentProxy>> Proxies;
};

}