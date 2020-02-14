#include "Actor.h"
#include "Components/WorldComponent.h"

void CActor::SetTransform(const STransform& InTransform)
{
	assert(!RootComponent.expired());

	RootComponent.lock()->SetTransform(InTransform);
}

void CActor::Destroy()
{
	for(const auto& [Name, Component] : ActorComponents)
	{
		Component->Destroy();
	}
}