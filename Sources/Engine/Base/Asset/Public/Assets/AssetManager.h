#pragma once

#include "EngineCore.h"
#include <filesystem>

namespace ZE { class CAsset; }

/**
 * Asset manager namespace
 * Provide a API to manage assets
 * Automatically cache assets when loaded
 * Assets are unloaded when no shared_ptr points to them
 */

namespace ZE::AssetManager
{

/** 
 * Get the specific asset 
 * If the asset isn't cached, it will load and cache it
 */
ASSET_API std::shared_ptr<CAsset> GetAsset(const std::filesystem::path& InPath);

/** Save the specified asset to disk */
ASSET_API bool SaveAsset(const std::shared_ptr<CAsset>& InAsset);

/** Templated version of GetAsset */
template<typename T>
ZE_FORCEINLINE std::shared_ptr<T> GetAsset(const std::filesystem::path& InPath)
{
	return std::static_pointer_cast<T>(GetAsset(InPath));
}

}