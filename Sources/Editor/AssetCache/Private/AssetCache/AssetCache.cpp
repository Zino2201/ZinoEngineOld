#include "AssetCache/AssetCache.h"
#include "ZEFS/Paths.h"
#include <filesystem>
#include "ZEFS/FileStream.h"

namespace ZE::AssetCache
{

std::filesystem::path CacheDir =
	ze::filesystem::get_current_working_dir() / std::filesystem::path("AssetCache");

void Cache(const std::string_view& InKey, const std::vector<uint8_t>& InData)
{
#if 0
	ze::FileSystem::COFileStream Stream(CacheDir / InKey,
		FileSystem::EFileWriteFlagBits::Binary |
		FileSystem::EFileWriteFlagBits::ReplaceExisting);
	if (!Stream)
	{
		ze::logger::error("Failed to cache key {}: can't open output stream", InKey);
		return;
	}

	Stream.write(reinterpret_cast<const char*>(InData.data()), 
		InData.size());

	ZE::Logger::Verbose("Cached asset key {}", InKey);
#endif
}

std::vector<uint8_t> Get(const std::string_view& InKey)
{
	return {};
}

bool HasKey(const std::string_view& InKey)
{
	return false;
}

}