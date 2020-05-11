#pragma once

#include "RenderableComponent.h"

namespace ZE::Components
{

/**
 * A static mesh component
 */
struct SStaticMeshComponent : public SRenderableComponent
{
	DECLARE_REFL_STRUCT_OR_CLASS1(SStaticMeshComponent, SRenderableComponent)

	//std::shared_ptr<CStaticMesh> StaticMesh;
	// TODO: Material
};
DECLARE_REFL_TYPE(SStaticMeshComponent);

}