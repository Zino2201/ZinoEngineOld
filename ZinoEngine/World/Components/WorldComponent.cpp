#include "WorldComponent.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<CActorComponent>("CWorldComponent")
		.constructor<>();
}

void CWorldComponent::SetTransform(const STransform& InTransform)
{
	Transform = InTransform;
}

void CWorldComponent::Attach(const std::weak_ptr<CWorldComponent>& InChildren)
{
	Childrens.push_back(InChildren);
}