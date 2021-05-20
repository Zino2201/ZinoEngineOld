#include "engine/ecs/SystemManager.h"
#include "engine/ecs/ComponentSystem.h"
#include "reflection/Class.h"

namespace ze
{

SystemManager::SystemManager(World& in_world) 
	: world(in_world)
{
	search_for_systems();
}

SystemManager::~SystemManager() = default;

void SystemManager::search_for_systems()
{
	for(const auto& c : reflection::Class::get_derived_classes_from(reflection::Class::get<ComponentSystem>()))
	{
		systems.emplace_back(c->instantiate<ComponentSystem>(world));
	}
}

}