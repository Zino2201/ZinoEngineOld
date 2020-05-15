#pragma once

#include "EngineCore.h"
#include "Render/RenderThreadResource.h"
#include "Render/UniformBuffer.h"

namespace ZE::Renderer
{

class CWorldProxy;

/**
 * Per-instance data of a renderable component
 */
struct SRenderableComponentPerInstanceData
{
    alignas(16) glm::mat4 World;
};

/**
 * Cache mode of a renderable component proxy
 * Defines if draw commands will be cached for this proxy
 */
enum class ERenderableComponentProxyCacheMode
{
    Cachable,
    Dynamic
};

/**
 * Render-thread version of a rendereable component
 * Class should be derived by rendereable components
 */
class RENDERER_API CRenderableComponentProxy : public CRenderThreadResource
{
public:
    CRenderableComponentProxy(CWorldProxy* InWorld,
        ERenderableComponentProxyCacheMode InCacheMode);
    virtual ~CRenderableComponentProxy();

    void InitResource_RenderThread() override;
    void DestroyResource_RenderThread() override;

    void SetTransform(const Math::STransform& InTransform) { Transform = InTransform; }
private:
    CWorldProxy* World;

    /** The transform of the component's entity */
    Math::STransform Transform;

    /** Cached per instance data, only updated when game thread version is modified */
    SRenderableComponentPerInstanceData PerInstanceData;

    TUniformBuffer<SRenderableComponentPerInstanceData> PerInstanceDataUBO;

    ERenderableComponentProxyCacheMode CacheMode;
};

}