#pragma once

#include "EngineCore.h"
#include "AssetPtr.h"

namespace ZE { class CAsset; }

/**
 * Asset manager namespace
 * Provide a API to manage assets
 * Automatically cache assets when loaded
 * Assets are unloaded when no shared_ptr points to them
 */

using tonassetsaved;
namespace ZE::AssetManager
{

/** Get all the assets in the specified path */
ENGINE_API void GetAssets(const std::string_view& InPath);

/** 
 * Get the specific asset 
 * If the asset isn't cached, it will load and cache it
 */
ENGINE_API std::shared_ptr<CAsset> GetAsset(const std::string_view& InPath);

/** Save the specified asset to disk */
ENGINE_API bool SaveAsset(const std::shared_ptr<CAsset>& InAsset);

/**
 * Remove the asset from the AssetManager's cache
 */
ENGINE_API void UncacheAsset(const TAssetPtr<CAsset>& InAsset);

/** Templated version of GetAsset */
template<typename T>
ZE_FORCEINLINE std::shared_ptr<T> GetAsset(const std::string_view& InPath)
{
	return std::static_pointer_cast<T>(GetAsset(InPath));
}

}