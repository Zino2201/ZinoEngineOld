#include "Engine/World.h"
#include "ECS.h"

namespace ZE
{

CWorld::CWorld()
	: EntityManager(std::make_unique<ECS::CEntityManager>())
{
}

void CWorld::Tick(float InDeltaTime)
{
	EntityManager->Tick(InDeltaTime);
}

}