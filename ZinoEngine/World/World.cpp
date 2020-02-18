#include "World.h"
#include "Actor.h"
#include "Render/Renderer/Scene.h"

CWorld::CWorld() : Scene(std::make_unique<CScene>(this)) {}
CWorld::~CWorld() {}

void CWorld::Tick(float InDeltaTime)
{
	for(const auto& Actor : Actors)
		Actor->Tick(InDeltaTime);
}

void CWorld::Destroy()
{
	for(const auto& Actor : Actors)
		Actor->Destroy();

	/** Release all actors */
	Actors.clear();
}