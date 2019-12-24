#pragma once

#include "ActorComponent.h"

/**
 * A component that has a transform and can be part of the world
 * Supports childrens
 */
class CWorldComponent : public CActorComponent
{
	REFLECTED_CLASS(CActorComponent)

public:
	void SetTransform(const STransform& InTransform);

	/**
	 * Attach specified component to this one
	 */
	void Attach(const std::weak_ptr<CWorldComponent>& InChildren);
private:
	STransform Transform;
	std::vector<std::weak_ptr<CWorldComponent>> Childrens;
};