#pragma once

#include "WorldComponent.h"

class CStaticMesh;

/**
 * Component that render a mesh
 */
class CStaticMeshComponent : public CWorldComponent
{
	REFLECTED_CLASS(CWorldComponent)

public:
	void SetStaticMesh(const std::shared_ptr<CStaticMesh>& InStaticMesh);

	const std::shared_ptr<CStaticMesh>& GetStaticMesh() const { return StaticMesh; }
private:
	std::shared_ptr<CStaticMesh> StaticMesh;
};