#include "World.h"
#include "Actor.h"

CWorld::CWorld() {}
CWorld::~CWorld() {}

void CWorld::Tick(float InDeltaTime)
{
	for(const auto& Actor : Actors)
		Actor->Tick(InDeltaTime);
}