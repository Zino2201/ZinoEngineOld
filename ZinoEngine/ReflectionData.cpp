/** Reflection data generated by ZinoReflectionTool, don't modify! */

#include "Core/EngineCore.h"
#include "./World/Actor.h"
#include "./World/Components/ActorComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<CActor>("CActor");
	rttr::registration::class_<CActorComponent>("CActorComponent")
		.property("Owner", &CActorComponent::Owner);
}
