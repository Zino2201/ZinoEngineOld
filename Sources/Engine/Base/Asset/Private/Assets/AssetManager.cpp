#include "Assets/AssetManager.h"
#include "Serialization/BinaryArchive.h"
#include "Assets/Asset.h"
#include <robin_hood.h>
#include "Reflection/Class.h"
#include <ostream>
#include "Module/Module.h"
#include "ZEFS/ZEFS.h"
#include <utility>
#include "Assets/Asset.h"
#include "ZEFS/FileStream.h"
#include "Reflection/Serialization.h"
#include "Threading/JobSystem/Async.h"
#include "Assets/AssetArchive.h"
#include "Assets/AssetMetadata.h"
#include "Assets/AssetCooker.h"
#include "Platform.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, Asset);

namespace std
{
	template<> struct hash<std::filesystem::path>
	{
		std::size_t operator()(const std::filesystem::path& in_path) const noexcept
		{
			return std::filesystem::hash_value(in_path);
		}
	};
}

namespace ze::assetmanager
{

struct AssetEntry
{
	std::unique_ptr<Asset> asset;
	std::atomic_uint32_t ref_count;

	AssetEntry() : ref_count(0) {}

	AssetEntry(const AssetEntry&) = delete;
	AssetEntry(AssetEntry&&) = delete;
	void operator=(const AssetEntry&) = delete;
	
	void operator=(AssetEntry&& other) 
	{
		asset = std::move(other.asset);
		ref_count = other.ref_count.load();
		other.ref_count = 0;
	}

	bool is_valid() const
	{
		return ref_count != 0;
	}
};

robin_hood::unordered_node_map<std::filesystem::path, AssetEntry> assets;

AssetRequestHandle::AssetRequestHandle(const std::vector<std::filesystem::path>& in_paths) : valid(true), paths(in_paths) {}

AssetRequestHandle::~AssetRequestHandle()
{
	if(!completed)
		return;

	for(const auto& path : paths)
	{
		AssetEntry& entry = assets[path];
		if(entry.is_valid())
		{
			entry.ref_count--;
		
			if(entry.ref_count == 0)
			{
				free_asset(path);
			}
		}
	}
}

Asset* AssetRequestHandle::get_asset() const
{
	ZE_CHECK(valid && completed);
	return assets[paths.front()].asset.get();
}

OwnerPtr<Asset> load_asset(const std::filesystem::path& in_path)
{
	/** Open the asset */
	filesystem::FileIStream stream(in_path, filesystem::FileReadFlagBits::Binary);
	if (!stream)
	{
		ze::logger::error("Failed to open asset {}", in_path.string());
		return nullptr;
	}

	/** Parse the header */
	auto metadata = get_metadata_from_file(in_path);
	if (!metadata.has_value())
	{
		ze::logger::error("Invalid metadata for asset {}", in_path.string());
		return nullptr; 
	}

	AssetInputArchive<serialization::BinaryInputArchive> ar(*metadata, stream);
	OwnerPtr<Asset> asset = metadata->asset_class->instantiate<Asset>();
	asset->set_path(in_path);
	asset->set_metadata(*metadata);
	reflection::serialization::serialize(ar, *asset);

	return asset;
}

std::pair<Asset*, std::shared_ptr<AssetRequestHandle>> load_asset_sync(const std::filesystem::path& in_path)
{
	std::shared_ptr<AssetRequestHandle> handle = std::make_shared<AssetRequestHandle>(std::vector<std::filesystem::path>{in_path});
	
	auto it = assets.find(in_path);
	if(it != assets.end())
	{
		handle->complete();
		it->second.ref_count++;
		return { it->second.asset.get(), handle };
	}

	ze::logger::verbose("Loading asset {}", in_path.string());

	OwnerPtr<Asset> asset = load_asset(in_path);
	if(!asset)
	{
		handle->cancel();
		ze::logger::error("Failed to load asset {}", in_path.string());
		return { nullptr, nullptr };
	}

	handle->complete();

	AssetEntry& asset_entry = assets[in_path];
	asset_entry.asset = std::unique_ptr<Asset>(asset);
	asset_entry.ref_count++;

	ze::logger::verbose("Loaded asset {}", in_path.string());

	return { asset_entry.asset.get(), handle };
}

std::shared_ptr<AssetRequestHandle> load_asset_async(const std::filesystem::path& in_path,
	std::function<AssetRequestHandle::OnCompletedFuncSignature>&& on_completed)
{
	std::shared_ptr<AssetRequestHandle> handle = std::make_shared<AssetRequestHandle>(std::vector<std::filesystem::path>{in_path});
	handle->bind_on_completed(std::move(on_completed));

	auto it = assets.find(in_path);
	if(it != assets.end())
	{
		it->second.ref_count++;
		handle->complete();
		return handle;
	}

	ze::logger::verbose("Loading asset {}", in_path.string());
	jobsystem::async([in_path, handle](const jobsystem::Job& in_job)
	{
		OwnerPtr<Asset> asset = load_asset(in_path);
		if(!asset)
		{
			ze::logger::verbose("Failed to load asset {}", in_path.string());
			handle->cancel();
		}

		AssetEntry& asset_entry = assets[in_path];
		asset_entry.asset = std::unique_ptr<Asset>(asset);
		asset_entry.ref_count++;
		ze::logger::verbose("Loaded asset {}", in_path.string());
		handle->complete();
	});

	return handle;
}

void free_asset(const std::filesystem::path& in_path)
{
	ze::logger::verbose("Unloading asset {}", in_path.string());
	assets.erase(in_path);
}

void unload_all()
{
	assets.clear();
}

std::optional<AssetMetadata> get_metadata_from_file(std::filesystem::path in_path)
{
	AssetMetadata metadata;

	/** Search a correspond metadata file, if we can't found one, the metadata should be stored in the asset file */
	std::filesystem::path metadata_path = in_path.concat("meta");
	if(ze::filesystem::exists(metadata_path))
	{
		/** Open the asset meta file */
		filesystem::FileIStream stream(metadata_path);
		if (!stream)
		{
			ze::logger::error("Failed to open asset metadata file {}", metadata_path.string());
			return std::nullopt;
		}
		
		serialization::JsonInputArchive ar(stream);
		ar <=> metadata;
		if (!metadata.asset_class)
		{
			ze::logger::error("Invalid asset {} (bad class)", in_path.string());
		}
		
		return std::make_optional(metadata);
	}
	else
	{
		
	}

	return std::nullopt;
}

void save_asset(const AssetSaveInfo& in_info)
{
#if ZE_WITH_EDITOR
	/** First cook the asset to the target platform */
	AssetCooker* cooker = AssetCooker::get_cooker_for(in_info.asset->get_class());
	AssetMetadata metadata = in_info.asset->get_metadata();
	AssetCookingContext cooker_ctx(in_info.asset, in_info.platform, metadata);
	logger::info("Saving asset {}...", in_info.asset->get_path().string(), in_info.platform.name);

	logger::info("Cooking asset {} for platform {}", in_info.asset->get_path().string(), in_info.platform.name);
	cooker->cook(cooker_ctx);

	/** Serialize the asset */
	{
		ze::filesystem::FileOStream stream(in_info.path, ze::filesystem::FileWriteFlagBits::Binary |
			ze::filesystem::FileWriteFlagBits::ReplaceExisting);
		if (!stream)
			return;

		AssetOutputArchive<serialization::BinaryOutputArchive> archive(metadata, stream);
		archive.cooked_data = std::move(cooker_ctx.cooked_data);
		reflection::serialization::serialize(archive, *in_info.asset);

		/** Save cooked data */
		if (metadata.has_seperate_cooked_data)
		{
			std::filesystem::path cooked_path = in_info.path;
			cooked_path.replace_extension("zecookeddata");
			ze::filesystem::FileOStream cooked_stream(cooked_path, ze::filesystem::FileWriteFlagBits::Binary |
				ze::filesystem::FileWriteFlagBits::ReplaceExisting);
			if (!cooked_stream)
				return;

			cooked_stream.write(reinterpret_cast<const char*>(archive.cooked_data.data()), archive.cooked_data.size());
		}
	}

	/** Save metadata */
	{
		std::filesystem::path meta_path = in_info.asset->get_path().concat("meta");
		ze::filesystem::FileOStream stream(meta_path,
			ze::filesystem::FileWriteFlagBits::ReplaceExisting);
		if (!stream)
			return;
		ze::filesystem::set_file_attributes(meta_path, ze::filesystem::FileAttributeFlagBits::Hidden);

		serialization::JsonOutputArchive archive(stream);
		archive <=> serialization::make_named_data("metadata", metadata);
	}
#else
	logger::error("Error! Trying to save {} in non-editor mode", asset->path.string());
#endif
}

}