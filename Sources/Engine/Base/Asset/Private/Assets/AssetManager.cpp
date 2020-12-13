#include "Assets/AssetManager.h"
#include "Serialization/BinaryArchive.h"
#include "Assets/Asset.h"
#include <robin_hood.h>
#include "Reflection/Class.h"
#include <ostream>
#include "Module/Module.h"
#include "ZEFS/FileStream.h"
#include "AssetDatabase/AssetDatabase.h"
#include <utility>
#include "Assets/Asset.h"
#include "AssetDatabase/AssetHeader.h"
#include "ZEFS/FileStream.h"
#include "Reflection/Serialization.h"
#include "Threading/JobSystem/Async.h"
#include "Assets/AssetArchive.h"

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

AssetRequestHandle::AssetRequestHandle(const std::vector<std::filesystem::path>& in_paths) : paths(in_paths),
	valid(true) {}

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
	AssetHeader header;
	ze::serialization::BinaryInputArchive ar(stream);
	ar <=> header;

	if (!header.is_valid())
	{
		ze::logger::error("Invalid asset {} (bad header)", in_path.string());
		return nullptr;
	}

	const reflection::Class* asset_class = ze::reflection::Class::get_by_name(header.class_name);
	if(!asset_class)
	{
		ze::logger::error("Invalid asset {} (bad class {})", in_path.string(), header.class_name);
		return nullptr;
	}

	OwnerPtr<Asset> asset = asset_class->instantiate<Asset>();
	asset->set_path(in_path);
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

}