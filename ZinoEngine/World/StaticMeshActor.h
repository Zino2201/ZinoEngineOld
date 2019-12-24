#pragma once

#include "Actor.h"

class CStaticMeshComponent;

/**
 * A actor that render a static mesh
 */
class CStaticMeshActor : public CActor
{
public:
	CStaticMeshActor();

	const std::shared_ptr<CStaticMeshComponent>& GetStaticMesh() const { return StaticMesh; }
private:
	std::shared_ptr<CStaticMeshComponent> StaticMesh;
};