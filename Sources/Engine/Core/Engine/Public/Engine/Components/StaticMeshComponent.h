#pragma once

#include "RenderableComponent.h"
#include "Renderer/RenderableComponentProxy.h"
#include "StaticMeshComponent.gen.h"

namespace ZE { class CStaticMesh; }

namespace ZE::Components
{

struct SStaticMeshComponent;

/**
 * A static mesh component
 */
ZSTRUCT()
struct SStaticMeshComponent : public SRenderableComponent
{
	REFL_BODY()

	std::shared_ptr<CStaticMesh> StaticMesh;

    void SetStaticMesh(const std::shared_ptr<CStaticMesh>& InSM);

    TOwnerPtr<Renderer::CRenderableComponentProxy> InstantiateProxy(
        Renderer::CWorldProxy* InWorld) const override;
};
}