#include "StaticMeshComponent.h"

void CStaticMeshComponent::SetStaticMesh(const std::shared_ptr<CStaticMesh>& InStaticMesh)
{
	StaticMesh = InStaticMesh;
}