#pragma once

#include "EngineCore.h"
#include "Engine/TickSystem.h"
#include "ComponentSystem.gen.h"

namespace ze
{

class World;

/*
 * A entity-component system that manage components
 * Each world has a system
 */
ZCLASS()
class ENGINE_API ComponentSystem : public ticksystem::Tickable
{
	ZE_REFL_BODY()

public:
	ComponentSystem(World& in_world) : world(in_world) 
	{
		can_tick = false;
		tick_flags = ticksystem::TickFlagBits::Variable;
	}

	virtual ~ComponentSystem() = default;
protected:
	World& world;
};

}