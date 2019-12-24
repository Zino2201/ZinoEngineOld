#include "StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

CStaticMeshActor::CStaticMeshActor()
{
	StaticMesh = CreateComponent<CStaticMeshComponent>("StaticMesh");

	RootComponent = StaticMesh;
}