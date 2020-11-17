#include "Engine/ECS/EntityManager.h"
#include "Reflection/Class.h"

namespace ze
{

EntityManager::EntityManager() : next(0)
{

}

Entity EntityManager::create()
{
	auto ent = entities.insert(Entity(++next));
	return *ent.first;
}

void EntityManager::destroy(Entity in_entity)
{
	entities.erase(in_entity);
}

}