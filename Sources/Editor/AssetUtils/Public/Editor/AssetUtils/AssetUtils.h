#pragma once

#include "EngineCore.h"
#include <filesystem>
#include "Delegates/Delegate.h"

namespace ZE { class CAsset; }

/**
 * Contains utilities functions for manipulating assets
 * Editor only!
 */
namespace ZE::Editor::AssetUtils
{

using TOnAssetImported = TDelegateNoRet<const std::filesystem::path&, const std::filesystem::path&>;

/**
 * Import assets with a dialog
 */
ASSETUTILS_API void ImportAssetsDialog(const std::filesystem::path& InPath,
	const std::filesystem::path& InTarget);

/**
 * Save the specified asset to disk
 */
ASSETUTILS_API void SaveAsset(ZE::CAsset& InAsset, const std::filesystem::path& InPath,
	const std::string& InName);

ASSETUTILS_API TOnAssetImported& GetOnAssetImported();

}