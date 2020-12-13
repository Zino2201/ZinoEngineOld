#include "AssetDataCache/AssetDataCache.h"
#include "ZEFS/Paths.h"
#include <filesystem>
#include "ZEFS/FileStream.h"
#include "ZEFS/ZEFS.h"
#include "Threading/JobSystem/Async.h"

namespace ze::assetdatacache
{

std::filesystem::path cache_dir = ze::filesystem::get_current_working_dir() / std::filesystem::path("AssetCache");

void cache(const std::string& in_context, const std::string& in_key, const std::vector<uint8_t>& in_data)
{
	if(!std::filesystem::exists(cache_dir))
		std::filesystem::create_directory(cache_dir);

	filesystem::FileOStream stream(cache_dir / in_key,
		filesystem::FileWriteFlagBits::Binary |
		filesystem::FileWriteFlagBits::ReplaceExisting);
	if (!stream)
	{
		logger::error("Failed to cache asset data {} key {}: can't open output stream", in_context, in_key);
		return;
	}

	stream.write(reinterpret_cast<const char*>(in_data.data()), in_data.size());

	logger::verbose("Cached asset data {} with key {}", in_context, in_key);
}

std::vector<uint8_t> get_sync(const std::string& in_key)
{
	if(!has_key(in_key))
		return {};

	filesystem::FileIStream stream(cache_dir / in_key,
		filesystem::FileReadFlagBits::Binary);

	stream.seekg(0, std::ios::end);
	int64_t size = stream.tellg();
	stream.seekg(0, std::ios::beg);

	std::vector<uint8_t> data;
	data.resize(size / sizeof(uint8_t));

	stream.read(reinterpret_cast<char*>(data.data()), data.size());

	return data;
}

std::future<std::vector<uint8_t>> get_async(const std::string& in_key)
{
	return jobsystem::async<std::vector<uint8_t>>(
		[in_key](const jobsystem::Job& in_job)
		{
			return get_sync(in_key);
		});
}

bool has_key(const std::string& in_key)
{
	return filesystem::exists(cache_dir / in_key);
}

}