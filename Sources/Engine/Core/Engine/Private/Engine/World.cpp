#include "Engine/World.h"
#include "Engine/ECS.h"

namespace ZE
{

CWorld::CWorld()
	: EntityManager(std::make_unique<ECS::CEntityManager>(*this))
{
}

void CWorld::Tick(float InDeltaTime)
{
	
}

}