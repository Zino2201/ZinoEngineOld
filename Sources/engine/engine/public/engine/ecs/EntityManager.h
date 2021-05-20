#pragma once

#include "ComponentManager.h"
#include "reflection/Cast.h"

namespace ze::reflection { class Class; }

namespace ze
{
/**
 * Manage a list of entities
 */
class ENGINE_API EntityManager final
{
public:
	EntityManager();

	EntityManager(const EntityManager&) = delete;
	void operator=(const EntityManager&) = delete;

	Entity create();

	ZE_FORCEINLINE bool contains(Entity in_entity) const { return entities.contains(in_entity); }
	void destroy(Entity in_entity);

	ZE_FORCEINLINE const auto& get_entities() const { return entities; }
private:
	robin_hood::unordered_flat_set<Entity> entities;
	uint64_t next;
};

}