#include "engine/components/SinusMovementSystem.h"
#include "engine/World.h"
#include "engine/components/TransformComponent.h"
#include "engine/Engine.h"

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
			float v = in_data.amplitude * std::sin(EngineApp::get_elapsed_time() * in_data.speed);
			switch(in_data.axis)
			{
			case MovementAxis::X:
				in_transform.position.x = v;
				break;
			case MovementAxis::Y:
				in_transform.position.y = v;
				break;
			case MovementAxis::Z:
				in_transform.position.z = v;
				break;
			}
		});
}

}