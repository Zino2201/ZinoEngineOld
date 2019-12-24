#include "Actor.h"
#include "Components/WorldComponent.h"

void CActor::SetTransform(const STransform& InTransform)
{
	assert(!RootComponent.expired());

	RootComponent.lock()->SetTransform(InTransform);
}