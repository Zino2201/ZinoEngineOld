#include "assetdatabase/AssetDatabase.h"
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
#include "Assets/AssetMetadata.h"
#include "Assets/AssetManager.h"

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
	data.meta_path = path;

	/** Metapath, if it exists */
	data.meta_path = data.meta_path.stem().concat(".zemeta");
	if(!ze::filesystem::exists(data.meta_path))
		data.meta_path = "";

	/** Parse the header */
	std::optional<AssetMetadata> metadata = assetmanager::get_metadata_from_file(path);
	if(!metadata.has_value())
	{
		ze::logger::error("Invalid metadata {}", path.string());
		return;
	}
	data.metadata = *metadata;

	path_tree.add(path);

	/** Open the asset file */
	{
		filesystem::FileIStream asset_stream(data.path,
			filesystem::FileReadFlagBits::End);
		if (!asset_stream)
		{
			ze::logger::error("Failed to open asset {}", path.string());
			return;
		}

		data.size = asset_stream.tellg();
	}

	data_map.insert({ path, data });
	ze::logger::verbose("Registered asset {} ({})", path.string(),
		metadata->asset_class->get_name());

	on_asset_registered.broadcast(data);
}

bool is_registered(const std::filesystem::path& path)
{
	return path_tree.has_path(path);
}

bool is_valid_asset(const std::filesystem::path& path)
{
	return path.extension() != ".zemeta" && assetmanager::get_metadata_from_file(path).has_value();
}

void scan_internal(const std::filesystem::path& path)
{
	filesystem::iterate_directories(path,
		[&](const filesystem::DirectoryEntry& entry)
		{
			if (filesystem::is_directory(path / entry.path) ||
				!is_valid_asset(path / entry.path) || 
				is_registered(entry.path))
				return;

			register_asset(path / entry.path);
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