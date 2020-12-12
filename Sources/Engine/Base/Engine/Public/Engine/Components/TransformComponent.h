#pragma once

#include "Engine/ECS/Component.h"
#include "Maths/Vector.h"
#include "Reflection/VectorRefl.h"
#include "TransformComponent.gen.h"

namespace ze
{

ZSTRUCT()
struct TransformComponent : public Component
{
	ZE_REFL_BODY()

	ZPROPERTY(Serializable, Visible, Editable)
	maths::Vector3d position;
	
	ZPROPERTY(Serializable, Visible, Editable)
	maths::Vector3f rotation;
	
	ZPROPERTY(Serializable, Visible, Editable)
	maths::Vector3f scale;

	TransformComponent() : scale(maths::Vector3f(1.f)) { }

	template<typename ArchiveType>
	void serialize(ArchiveType& in_archive)
	{
		in_archive <=> position;
		in_archive <=> rotation;
		in_archive <=> scale;
	}

	ZE_FORCEINLINE bool operator==(const TransformComponent& other) const
	{
		return position == other.position &&
			rotation == other.rotation &&
			scale == other.scale;
	}

	ZE_FORCEINLINE bool operator!=(const TransformComponent& other) const
	{
		return position != other.position ||
			rotation != other.rotation ||
			scale != other.scale;
	}
};

}