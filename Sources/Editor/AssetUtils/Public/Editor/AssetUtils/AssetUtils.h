#pragma once

#include "EngineCore.h"
#include <filesystem>
#include "Delegates/Delegate.h"

namespace ze { class Asset; }

/**
 * Contains utilities functions for manipulating assets
 * Editor only!
 */
namespace ze::editor::assetutils
{

using OnAssetImported = ze::DelegateNoRet<const std::filesystem::path&, const std::filesystem::path&>;

/**
 * Import assets with a dialog
 */
ASSETUTILS_API void import_assets_dialog(const std::filesystem::path& in_path,
	const std::filesystem::path& in_target);

ASSETUTILS_API void save_asset(ze::Asset& in_asset);

/**
 * Save the specified asset to disk
 */
ASSETUTILS_API void save_asset(ze::Asset& in_asset, const std::filesystem::path& in_path,
	const std::string& in_name);

ASSETUTILS_API OnAssetImported& get_on_asset_imported();

}