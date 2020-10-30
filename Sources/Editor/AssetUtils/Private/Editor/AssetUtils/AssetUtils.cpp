#include "Editor/AssetUtils/AssetUtils.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>
#endif
#include "Reflection/Serialization.h"
#include "ZEFS/FileStream.h"
#include "ZEFS/Paths.h"
#include "Serialization/BinaryArchive.h"
#include "AssetDatabase/AssetHeader.h"
#include "Reflection/ArchivesFwd.h"
#include "Assets/Asset.h"

namespace ze::editor::assetutils
{
	
OnAssetImported on_asset_imported;	

void import_assets_dialog(const std::filesystem::path& in_path,
	const std::filesystem::path& in_target)
{
#if ZE_PLATFORM(WINDOWS)
	OPENFILENAME fn;
	
	char filebuf[256] = "";
	char cwd[256] = "";
	std::string path = in_path.string();

	GetCurrentDirectoryA(256, cwd);

	ZeroMemory(&fn, sizeof(fn));
	fn.lStructSize = sizeof(fn);
	fn.hwndOwner = nullptr;
	fn.lpstrFile = filebuf;
	fn.nMaxFile = sizeof(filebuf);
	fn.lpstrFileTitle = nullptr;
	fn.nMaxFileTitle = 0;
	fn.lpstrInitialDir = path.c_str();
	fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&fn) == TRUE)
	{
		std::filesystem::path file = filebuf;
		SetCurrentDirectory(cwd);
		on_asset_imported.execute(file, in_target);
	}

	SetCurrentDirectory(cwd);
#endif
}

void save_asset(ze::Asset& in_asset, const std::filesystem::path& in_path,
	const std::string& in_name)
{
	std::string filename = in_name + ".zasset";
	ze::filesystem::FileOStream stream(in_path / filename,
		ze::filesystem::FileWriteFlagBits::Binary |
		ze::filesystem::FileWriteFlagBits::ReplaceExisting);
	if (!stream)
		return;

	ZE_CHECK(false);

	//Serialization::COBinaryArchive Archive(Stream);
	//Archive <=> MakeAssetHeader(InAsset.GetClass()->GetName());
	//ze::reflection::serialization::serialize(Archive, InAsset);
}

OnAssetImported& get_on_asset_imported() { return on_asset_imported; }

}
