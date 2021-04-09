#include "AssetDatabase/AssetDatabase.h"
#include "EngineCore.h"
#include <robin_hood.h>
#include "Module/ModuleManager.h"
#include "Threading/JobSystem/Async.h"
#include "AssetDatabase/AssetHeader.h"
#include <istream>
#include <ios>
#include "Serialization/BinaryArchive.h"
#include "Reflection/Class.h"
#include "Module/Module.h"
#include "ZEFS/FileStream.h"
#include "ZEFS/ZEFS.h"
#include "PathTree.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, AssetDatabase);

namespace ze::assetdatabase
{

PathTree path_tree;
robin_hood::unordered_map<std::filesystem::path, AssetPrimitiveData> data_map;
OnAssetRegistered on_asset_registered;
OnAssetScanCompleted on_asset_scan_completed;
std::mutex map_mutex;

void register_asset(const std::filesystem::path& path)
{
	std::lock_guard<std::mutex> guard(map_mutex);

	AssetPrimitiveData data;
	data.name = path.stem().string();
	data.path = path;

	/** Open the asset */
	filesystem::FileIStream stream(path, filesystem::FileReadFlagBits::Binary |
		filesystem::FileReadFlagBits::End);
	if (!stream)
	{
		ze::logger::error("Failed to open asset {}", path.string());
		return;
	}

	/** Get size */
	data.size = stream.tellg();
	stream.seekg(0, std::ios::beg);

	/** Parse the header */
	AssetHeader header;
	ze::serialization::BinaryInputArchive ar(stream);
	ar <=> header;

	if (!header.is_valid())
	{
		ze::logger::error("Invalid asset {} (bad header)", path.string());
		return;
	}

	data.engine_ver = header.engine_ver;
	data.asset_class = ze::reflection::Class::get_by_name(header.class_name);
	if(!data.asset_class)
	{
		ze::logger::error("Invalid asset {} (bad class {})", path.string(), header.class_name);
		return;
	}

	path_tree.add(path);
	data_map.insert({ path , data });

	ze::logger::verbose("Registered asset {} ({})", path.string(),
		header.class_name);

	on_asset_registered.broadcast(data);
}

bool is_registered(const std::filesystem::path& path)
{
	return path_tree.has_path(path);
}

bool is_valid_asset(const std::filesystem::path& path)
{
	return path.extension().string() == ".zeasset";
}

void scan_internal(const std::filesystem::path& path)
{
	filesystem::iterate_directories(path,
		[&](const filesystem::DirectoryEntry& entry)
		{
			if (filesystem::is_directory(path / entry.path) ||
				!is_valid_asset(entry.path) || 
				is_registered(entry.path))
				return;

			register_asset(path/ entry.path);
		}, filesystem::IterateDirectoriesFlagBits::Recursive);

	on_asset_scan_completed.broadcast();
}

void scan(const std::filesystem::path& path, const AssetScanMode& scanmode)
{
	switch (scanmode)
	{
	case AssetScanMode::Async:
		ze::jobsystem::async(
			[path](const ze::jobsystem::Job& in_job)
			{
				scan_internal(path);
			});
		break;
	case AssetScanMode::Sync:
		scan_internal(path);
		break;
	}
}

std::vector<AssetPrimitiveData> get_assets(const std::filesystem::path& dir)
{
	auto childs = path_tree.get_childs(dir, true);
	
	std::vector<AssetPrimitiveData> assets;
	assets.reserve(childs.size());

	for (const auto& child : childs)
	{
		auto it = data_map.find(dir / child);
		if (it != data_map.end())
			assets.emplace_back(it->second);
	}

	return assets;
}

std::optional<AssetPrimitiveData> get_asset_primitive_data(const std::filesystem::path& path)
{
	auto data = data_map.find(path);
	if (data != data_map.end())
		return data->second;

	return std::nullopt;
}

std::vector<std::filesystem::path> get_subdirectories(const std::filesystem::path& root)
{
	return path_tree.get_childs(root, false);
}

OnAssetRegistered& get_on_asset_registered() { return on_asset_registered; }
OnAssetScanCompleted& get_on_asset_scan_completed() { return on_asset_scan_completed; }

}