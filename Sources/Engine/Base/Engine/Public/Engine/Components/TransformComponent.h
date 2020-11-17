#pragma once

#include "Engine/ECS/Component.h"
#include "Maths/Vector.h"
#include "TransformComponent.gen.h"

namespace ze
{

ZSTRUCT()
struct TransformComponent : public Component
{
	ZE_REFL_BODY()

	ZPROPERTY(Serializable, Editable)
	maths::Vector3d position;
	
	ZPROPERTY(Serializable, Editable)
	maths::Vector3f rotation;
	
	ZPROPERTY(Serializable, Editable)
	maths::Vector3f scale;

	TransformComponent() : scale(maths::Vector3f(1.f)) { }
};

}