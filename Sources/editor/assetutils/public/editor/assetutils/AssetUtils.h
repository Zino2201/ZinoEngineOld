#pragma once

#include "EngineCore.h"
#include <filesystem>
#include "delegates/Delegate.h"

namespace ze { class Asset; struct AssetMetadata; }

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
void import_assets_dialog(const std::filesystem::path& in_path,
	const std::filesystem::path& in_target);

void save_asset(const std::filesystem::path& in_file_path);

OnAssetImported& get_on_asset_imported();

}