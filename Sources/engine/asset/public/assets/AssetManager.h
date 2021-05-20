#pragma once

#include "EngineCore.h"
#include "delegates/Delegate.h"
#include "Asset.h"
#include <filesystem>
#include <future>
#include <optional>

namespace ze { class Asset; struct AssetMetadata; struct PlatformInfo; }

/**
 * Asset manager namespace
 * Provide a API to manage assets
 * Automatically cache assets when loaded
 * Assets are unloaded when no shared_ptr points to them
 */
namespace ze::assetmanager
{

/**
 * A handle to a load request of single/group of assets given by the asset manager
 * Will keep alive the asset as long as this asset has handles referencing it
 */
class ASSET_API AssetRequestHandle
{
public:
	using OnCompletedFuncSignature = void();

	AssetRequestHandle(const std::vector<std::filesystem::path>& in_paths);
	~AssetRequestHandle();

	void bind_on_completed(std::function<OnCompletedFuncSignature>&& in_func)
	{
		if(completed)
		{
			in_func();
		}
		else
		{
			on_completed.bind(std::move(in_func));
		}
	}

	void complete()
	{
		completed = true;
		on_completed.execute();
	}

	void cancel()
	{
		valid = false;
	}

	/** Is the request valid ? (the request was not canceled or the asset unloaded) */
	ZE_FORCEINLINE bool is_valid() const { return valid; }
	ZE_FORCEINLINE bool is_loading() const { return valid && !completed; }
	
	/** Will return the asset of the first path stored in this request */
	Asset* get_asset() const;
private:
	std::atomic_bool valid;
	std::atomic_bool completed;

	std::vector<std::filesystem::path> paths;
	Delegate<void> on_completed;
};

/** Shared pointer to a asset request handle, allows an asset to be kept alive until no other requests are left */
using AssetRequestPtr = std::shared_ptr<assetmanager::AssetRequestHandle>;

ASSET_API std::pair<Asset*, AssetRequestPtr> load_asset_sync(const std::filesystem::path& in_path);

/**
 * Load an asset asynchronously 
 * Call on_completed when finished
 * \warning on_completed maybe called before load_asset_async returns (e.g asset already loaded)
 */
ASSET_API AssetRequestPtr load_asset_async(const std::filesystem::path& in_path,
	std::function<AssetRequestHandle::OnCompletedFuncSignature>&& on_completed = nullptr);
ASSET_API void free_asset(const std::filesystem::path& in_path);
ASSET_API void unload_all();

/** Save information for assets */
struct AssetSaveInfo
{
	Asset* asset;
	const PlatformInfo& platform;
	std::filesystem::path path;

	AssetSaveInfo(Asset* in_asset,
		const PlatformInfo& in_platform) : asset(in_asset), platform(in_platform), path(asset->get_path()) {}
};

/*
 * Save an asset to disk, perform cooking and also save the metadata
 * (Editor only!)
 */
void save_asset(const AssetSaveInfo& in_info);

/**
 * Try get the metadata of the specified file if this is an asset
 * This function works with cooked asset (single asset file) and uncooked assets (asset file and .zemeta file)
 */
std::optional<AssetMetadata> get_metadata_from_file(std::filesystem::path in_path);

}