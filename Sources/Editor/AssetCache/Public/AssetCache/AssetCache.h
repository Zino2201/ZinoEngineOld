#pragma once

#include "EngineCore.h"
#include <string_view>
#include <vector>

/**
 * Asset cache API
 * The asset cache store some asset data like material shaders
 *	and textures so that this data doesn't need to be embedded to the asset file
 *	and to allow working on a project on multiple platforms with different needs
 */
namespace ZE::AssetCache
{

/**
 * Cache the specified data to the asset cache with
 * the following key
 */
ASSETCACHE_API void Cache(const std::string_view& InKey,
	const std::vector<uint8_t>& InData);

/**
 * Get the data
 */
ASSETCACHE_API std::vector<uint8_t> Get(const std::string_view& InKey);

/**
 * Returns true if the cache contains the key
 */
ASSETCACHE_API bool HasKey(const std::string_view& InKey);

}