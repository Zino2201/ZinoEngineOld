#pragma once

#include "MinimalMacros.h"
#include "Delegates/MulticastDelegate.h"
#include "Assets/AssetMetadata.h"
#include <filesystem>
#include <unordered_set>
#include <optional>
#include "EngineVer.h"

namespace ze::reflection { class Class; }

/*
 * Asset database
 * Contains a path tree with a list of all assets
 * Allow retrieving informations about assets without loading them
 */
namespace ze::assetdatabase
{

const std::string metafile_extension = ".zemeta";

/**
 * Primitive informations about an asset
 * This data is available in every configuration (with Editor/no Editor)
 *	as this data is gathered from the meta file
 */
struct AssetPrimitiveData
{
	/** Asset name */
	std::string name;

	/** Size of the asset (in bytes) */
	uint64_t size;

	/** Metadata */
	AssetMetadata metadata;

	std::filesystem::path path;
	std::filesystem::path meta_path;

	AssetPrimitiveData() : size(0) {}
};

/**
 * Asset scan mode
 */
enum class AssetScanMode
{
	Async,
	Sync
};

/** Delegates */
using OnAssetRegistered = MulticastDelegateNoRet<const AssetPrimitiveData&>;
using OnAssetScanCompleted = MulticastDelegateNoRet<>;

/** Functions */

/**
 * Scan a directory or file for asset(s)
 * By default, this scan is asynchronous 
 */
ASSETDATABASE_API void scan(const std::filesystem::path& path, const AssetScanMode& scanmode = AssetScanMode::Async);

/**
 * Get sub directories of the specified directory in the asset database path tree
 */
ASSETDATABASE_API std::vector<std::filesystem::path> get_subdirectories(const std::filesystem::path& rppt);

/**
 * Get assets contained in this directory
 */
ASSETDATABASE_API std::vector<AssetPrimitiveData> get_assets(const std::filesystem::path& dir);

/**
 * Get the specified asset primitive data
 */
ASSETDATABASE_API std::optional<AssetPrimitiveData> get_asset_primitive_data(const std::filesystem::path& path);

ASSETDATABASE_API inline OnAssetRegistered& get_on_asset_registered();
ASSETDATABASE_API inline OnAssetScanCompleted& get_on_asset_scan_completed();

}