#include "ActorComponent.h"
#include "World/Actor.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<CActorComponent>("CActorComponent")
		.constructor<>()
		.property("Owner", &CActorComponent::Owner);
}