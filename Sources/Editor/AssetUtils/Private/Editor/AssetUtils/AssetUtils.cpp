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

namespace ZE::Editor::AssetUtils
{
	
TOnAssetImported OnAssetImported;	

void ImportAssetsDialog(const std::filesystem::path& InPath,
	const std::filesystem::path& InTarget)
{
#if ZE_PLATFORM(WINDOWS)
	OPENFILENAME Fn;
	
	char FileBuf[256] = "";
	char CWD[256] = "";
	std::string Path = InPath.string();

	GetCurrentDirectoryA(256, CWD);

	ZeroMemory(&Fn, sizeof(Fn));
	Fn.lStructSize = sizeof(Fn);
	Fn.hwndOwner = nullptr;
	Fn.lpstrFile = FileBuf;
	Fn.nMaxFile = sizeof(FileBuf);
	Fn.lpstrFileTitle = nullptr;
	Fn.nMaxFileTitle = 0;
	Fn.lpstrInitialDir = Path.c_str();
	Fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&Fn) == TRUE)
	{
		std::filesystem::path File = FileBuf;
		SetCurrentDirectory(CWD);
		OnAssetImported.Execute(File, InTarget);
	}

	SetCurrentDirectory(CWD);
#endif
}

void SaveAsset(ZE::CAsset& InAsset, const std::filesystem::path& InPath,
	const std::string& InName)
{
	std::string FinalName = InName + ".zasset";
	FileSystem::COFileStream Stream(InPath / FinalName,
		FileSystem::EFileWriteFlagBits::Binary |
		FileSystem::EFileWriteFlagBits::ReplaceExisting);
	if (!Stream)
		return;

	Serialization::COBinaryArchive Archive(Stream);
	//Archive <=> MakeAssetHeader(InAsset.GetClass()->GetName());
	//ze::reflection::serialization::serialize(Archive, InAsset);
}

TOnAssetImported& GetOnAssetImported() { return OnAssetImported; }

}