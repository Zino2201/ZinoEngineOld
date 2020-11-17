#pragma once

#include "EngineCore.h"
#include <robin_hood.h>
#include "ECS.gen.h"

namespace ze
{

ZSTRUCT()
struct Entity
{
	ZE_REFL_BODY()

	static constexpr uint64_t null = 0;

	uint64_t id;

	Entity() : id(null) {}
	explicit Entity(const uint64_t in_id) : id(in_id) {}

	ZE_FORCEINLINE bool operator==(const Entity& other) const { return id == other.id; }
	ZE_FORCEINLINE bool operator!=(const Entity& other) const { return id != other.id; }

	ZE_FORCEINLINE bool operator==(std::nullptr_t) const { return id == null; }
	ZE_FORCEINLINE bool operator!=(std::nullptr_t) const { return id != null; }
};

}

namespace std
{
	template<> struct hash<ze::Entity>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::Entity in_entity) const
		{
			return std::hash<uint64_t>()(in_entity.id);
		}
	};
}