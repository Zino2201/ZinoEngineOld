#pragma once

#include "EngineCore.h"

namespace ze
{

class ComponentSystem;
class World;

/**
 * Manage mutliple systems for a single world
 */
class ENGINE_API SystemManager
{
public:
	SystemManager(World& in_world);
	~SystemManager();

	SystemManager(const SystemManager&) = delete;
	void operator=(const SystemManager&) = delete;
private:
	void search_for_systems();
private:
	std::vector<std::unique_ptr<ComponentSystem>> systems;
	World& world;
};

}