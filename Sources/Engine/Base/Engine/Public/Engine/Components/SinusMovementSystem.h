#pragma once

#include "Engine/ECS/ComponentSystem.h"
#include "Engine/ECS/Component.h"
#include "SinusMovementSystem.gen.h"

namespace ze
{

ZENUM()
enum class MovementAxis
{
	X,
	Y,
	Z
};

ZSTRUCT()
struct SinusMovementDataComponent : public Component
{
	ZE_REFL_BODY()

	ZPROPERTY(Editable)
	float amplitude;

	ZPROPERTY(Editable)
	float speed;

	ZPROPERTY(Editable)
	MovementAxis axis;

	SinusMovementDataComponent() : amplitude(2.f), speed(2), axis(MovementAxis::Y) {}
};

ZCLASS()
class ENGINE_API SinusMovementSystem : public ComponentSystem
{
	ZE_REFL_BODY()

public:
	SinusMovementSystem(World& in_world);

	void variable_tick(const float in_delta_time) override;
};

}