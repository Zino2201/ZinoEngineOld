#include "Engine/Components/SinusMovementSystem.h"
#include "Engine/World.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Engine.h"

namespace ze
{

SinusMovementSystem::SinusMovementSystem(World& in_world)
	: ComponentSystem(in_world)
{
	can_tick = true;
}

void SinusMovementSystem::variable_tick(const float in_delta_time)
{
	world.for_each<TransformComponent, SinusMovementDataComponent>(
		[](Entity in_entity, TransformComponent& in_transform, 
			SinusMovementDataComponent& in_data)
		{
			in_transform.position.y = in_data.amplitude * std::sin(EngineApp::get_elapsed_time() * in_data.speed);
		});
}

}