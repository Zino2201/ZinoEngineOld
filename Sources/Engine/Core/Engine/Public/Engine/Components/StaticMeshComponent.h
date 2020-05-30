#pragma once

#include "RenderableComponent.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ZE { class CStaticMesh; }

namespace ZE::Components
{

struct SStaticMeshComponent;

/**
 * A static mesh component
 */
struct SStaticMeshComponent : public SRenderableComponent
{
	DECLARE_REFL_STRUCT_OR_CLASS1(SStaticMeshComponent, SRenderableComponent)

	std::shared_ptr<CStaticMesh> StaticMesh;

    void SetStaticMesh(const std::shared_ptr<CStaticMesh>& InSM);

    TOwnerPtr<Renderer::CRenderableComponentProxy> InstantiateProxy(
        Renderer::CWorldProxy* InWorld) const override;
};
DECLARE_REFL_TYPE(SStaticMeshComponent);

}