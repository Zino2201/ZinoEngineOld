#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"

/**
 * Base asset manager
 * Manages all assets
 */
class CAssetManager : public CNonCopyable
{
public:
	static CAssetManager& Get()
	{
		static CAssetManager Instance;
		return Instance;
	}
private:
	
};