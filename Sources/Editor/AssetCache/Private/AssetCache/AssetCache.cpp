#include "AssetCache/AssetCache.h"
#include "ZEFS/Paths.h"
#include <filesystem>
#include "ZEFS/FileStream.h"

namespace ZE::AssetCache
{

std::filesystem::path CacheDir =
	FileSystem::Paths::GetCurrentWorkingDir() / std::filesystem::path("AssetCache");

void Cache(const std::string_view& InKey, const std::vector<uint8_t>& InData)
{
	FileSystem::COFileStream Stream(CacheDir / InKey,
		FileSystem::EFileWriteFlagBits::Binary |
		FileSystem::EFileWriteFlagBits::ReplaceExisting);
	if (!Stream)
	{
		ZE::Logger::Error("Failed to cache key {}: can't open output stream", InKey);
		return;
	}

	Stream.write(reinterpret_cast<const char*>(InData.data()), 
		InData.size());

	ZE::Logger::Verbose("Cached asset key {}", InKey);
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