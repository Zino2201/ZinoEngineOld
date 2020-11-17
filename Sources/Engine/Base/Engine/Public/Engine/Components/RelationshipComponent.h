#pragma once

#include "Engine/ECS/ComponentManager.h"
#include "RelationshipComponent.gen.h"

namespace ze
{

ZSTRUCT(HideInEditor)
struct RelationshipComponent : public Component
{
	ZE_REFL_BODY()

	ZPROPERTY(Serializable)
	uint32_t childrens;

	ZPROPERTY(Serializable)
	Entity first;
	
	ZPROPERTY(Serializable)
	Entity prev;
	
	ZPROPERTY(Serializable)
	Entity next;
	
	ZPROPERTY(Serializable)
	Entity parent;
};

}