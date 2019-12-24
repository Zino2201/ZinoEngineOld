#pragma once

#include "EngineCore.h"

/**
 * Base class for tickable objects
 */
class ITickableObject
{
public:
	virtual void Tick(float InDeltaTime) = 0;
};