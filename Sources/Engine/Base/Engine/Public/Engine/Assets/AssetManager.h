#pragma once

#include "EngineCore.h"

namespace ZE { class CAsset; }

/**
 * Asset manager namespace
 * Provide a API to manage assets
 * Assets must be cached first before being accessed
 */
namespace ZE::AssetManager
{

/** Search for assets in the specified path and cache them to the asset manager */
ENGINE_API uint32_t SearchAndCacheAssets(const std::string_view& InPath);

/** Get all the assets in the specified path */
ENGINE_API void GetAssets(const std::string_view& InPath);

/** Get the specific asset, this will load it  */
ENGINE_API std::shared_ptr<CAsset> GetAsset(const std::string_view& InPath);

/** Templated version of GetAsset */
template<typename T>
FORCEINLINE std::shared_ptr<T> GetAsset(const std::string_view& InPath)
{
	return std::static_pointer_cast<T>(GetAsset(InPath));
}

}