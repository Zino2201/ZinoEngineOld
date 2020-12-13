#pragma once

#include "EngineCore.h"
#include <string_view>
#include <vector>
#include <future>

/**
 * Asset cache API
 * This allows asset to store additional data that can't be embedded directly into the asset file
 *	or data that is platform-specific
 */
namespace ze::assetdatacache
{

/**
 * Cache the specified data to the asset cache with the following key
 */
ASSETDATACACHE_API void cache(const std::string& in_context, const std::string& in_key, const std::vector<uint8_t>& in_data);

/**
 * Get the data
 */
ASSETDATACACHE_API std::vector<uint8_t> get_sync(const std::string& in_key);

/**
 * Get the data (async version)
 * \return Future object to data
 */
ASSETDATACACHE_API std::future<std::vector<uint8_t>> get_async(const std::string& in_key);

/**
 * Returns true if the cache contains the key
 */
ASSETDATACACHE_API bool has_key(const std::string& in_key);

}