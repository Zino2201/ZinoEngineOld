#pragma once

#include "EngineCore.h"

/**
 * Base asset class
 */
class IAsset
{
public:
	virtual ~IAsset() = default;

	virtual void Load(const std::string& InPath) = 0;
};