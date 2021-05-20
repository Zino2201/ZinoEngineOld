#pragma once

#include "ecs/EntityManager.h"
#include "ecs/ComponentManager.h"
#include "ecs/SystemManager.h"
#include "Engine/TickSystem.h"
#include "World.gen.h"

namespace ze
{

/**
 * A world, a container of entities
 * There can be multiples worlds
 */
ZCLASS()
class ENGINE_API World : public ticksystem::Tickable
{
	ZE_REFL_BODY()
		
public:
	World();
	~World();

	Entity spawn_entity();
	void destroy_entity(Entity in_entity);

	void add_component(Entity in_entity, const reflection::Class* in_type);
	void remove_component(Entity in_entity, const reflection::Class* in_type);

	template<typename T, typename... Args>
	ZE_FORCEINLINE T& add_component(Entity in_entity, Args&&... in_args)
	{
		return component_mgr.add_component<T, Args...>(in_entity, std::forward<Args>(in_args)...);
	}

	template<typename... Types, typename Lambda>
	void for_each(Lambda in_lambda)
	{
		std::vector<const reflection::Class*> required_classes;
		required_classes.emplace_back((reflection::Class::get<Types>(), ...));

		for(const auto& archetype : component_mgr.get_archetypes())
		{
			if(archetype->id.has(required_classes))
			{
				for(const auto& entity : archetype->entities)
				{
					in_lambda(entity, component_mgr.get_component<Types>(entity)...);
				}
			}
		}
	}

	ZE_FORCEINLINE EntityManager& get_entity_mgr() { return entity_mgr; }
	ZE_FORCEINLINE ComponentManager& get_component_mgr() { return component_mgr; }
private:
	EntityManager entity_mgr;
	ComponentManager component_mgr;
	SystemManager system_mgr;
};

}