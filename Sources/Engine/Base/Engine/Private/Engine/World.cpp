#include "Engine/World.h"

namespace ze
{

World::World()
	: system_mgr(*this)
{

}

World::~World() = default;

Entity World::spawn_entity()
{
	Entity ent = entity_mgr.create();
	component_mgr.register_entity(ent);
	return ent;
}

void World::destroy_entity(Entity in_entity)
{
	entity_mgr.destroy(in_entity);
	component_mgr.unregister_entity(in_entity);
}

}