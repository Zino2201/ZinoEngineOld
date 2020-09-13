#pragma once

#include "MinimalMacros.h"
#include "Delegates/MulticastDelegate.h"
#include <filesystem>
#include <unordered_set>

namespace ZE::Refl { class CClass; }

/*
 * Asset database
 * Contains a path tree with a list of all assets
 * Allow retrieving informations about assets without loading them
 */
namespace ZE::AssetDatabase
{

/**
 * Primitive informations about an asset
 * This data is available in every configuration (with Editor/no Editor)
 *	as this data is gathered from the zasset header and/or file metadata
 */
struct SAssetPrimitiveData
{
	/** Asset name */
	std::string Name;
	
	/** Asset class */
	Refl::CClass* Class;

	/** Size of the asset (in bytes) */
	uint64_t Size;

	std::filesystem::path Path;

	SAssetPrimitiveData() : Class(nullptr), Size(0) {}
};

/**
 * Asset scan mode
 */
enum class EAssetScanMode
{
	/** Scan in another thread */
	Async,
	Sync
};

/** Delegates */
using TOnAssetRegistered = TMulticastDelegateNoRet<const SAssetPrimitiveData&>;
using TOnAssetScanCompleted = TMulticastDelegateNoRet<>;

/** Functions */

/**
 * Scan a directory or file for asset(s)
 * By default, this scan is asynchronous 
 */
ASSETDATABASE_API void Scan(const std::filesystem::path& InPath, const EAssetScanMode& InScanMode = EAssetScanMode::Async);

/**
 * Get sub directories of the specified directory in the asset database path tree
 */
ASSETDATABASE_API std::vector<std::filesystem::path> GetSubDirectories(const std::filesystem::path& InRoot);

/**
 * Get assets contained in this directory
 */
ASSETDATABASE_API std::vector<SAssetPrimitiveData> GetAssets(const std::filesystem::path& InDirectory);

ASSETDATABASE_API inline TOnAssetRegistered& GetOnAssetRegistered();
ASSETDATABASE_API inline TOnAssetScanCompleted& GetOnAssetScanCompleted();

}