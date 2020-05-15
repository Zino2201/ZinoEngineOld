#pragma once

#include "RenderableComponent.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ZE::Components
{

/**
 * Proxy for a static mesh component
 */
class ENGINE_API CStaticMeshComponentProxy : public Renderer::CRenderableComponentProxy
{
public:
    CStaticMeshComponentProxy(Renderer::CWorldProxy* InWorld) :
        Renderer::CRenderableComponentProxy(InWorld,
            Renderer::ERenderableComponentProxyCacheMode::Cachable) {}
};

/**
 * A static mesh component
 */
struct SStaticMeshComponent : public SRenderableComponent
{
	DECLARE_REFL_STRUCT_OR_CLASS1(SStaticMeshComponent, SRenderableComponent)

	//std::shared_ptr<CStaticMesh> StaticMesh;
	// TODO: Material

    Renderer::CRenderableComponentProxy* InstantiateProxy(Renderer::CWorldProxy* InWorld) const override 
    {
        return new CStaticMeshComponentProxy(InWorld); 
    }
};
DECLARE_REFL_TYPE(SStaticMeshComponent);

}