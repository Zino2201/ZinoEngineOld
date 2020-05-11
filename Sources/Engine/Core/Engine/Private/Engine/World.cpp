#include "Engine/World.h"
#include "Engine/ECS.h"
#include "Renderer/WorldProxy.h"

namespace ZE
{

CWorld::CWorld()
	: EntityManager(std::make_unique<ECS::CEntityManager>(*this)),
	Proxy(std::make_unique<Renderer::CWorldProxy>())
{

}

CWorld::~CWorld() = default;

void CWorld::Tick(float InDeltaTime)
{
	__debugbreak();
}

}