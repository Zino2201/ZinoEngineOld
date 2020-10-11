#include "AssetDatabase/AssetDatabase.h"
#include "EngineCore.h"
#include <robin_hood.h>
#include "Module/ModuleManager.h"
#include "Threading/JobSystem/Async.h"
#include "AssetDatabase/AssetHeader.h"
#include <istream>
#include <ios>
#include "Serialization/BinaryArchive.h"
#include "Reflection/Class.h"
#include "Module/Module.h"
#include "ZEFS/FileStream.h"
#include "ZEFS/ZEFS.h"
#include "PathTree.h"

DEFINE_MODULE(ZE::Module::CDefaultModule, AssetDatabase);

namespace ZE::AssetDatabase
{

CPathTree PathTree;
robin_hood::unordered_map<std::filesystem::path, SAssetPrimitiveData> DataMap;

/** Delegates */
TOnAssetRegistered OnAssetRegistered;
TOnAssetScanCompleted OnAssetScanCompleted;

std::mutex MapMutex;

void RegisterAsset(const std::filesystem::path& InPath)
{
	std::lock_guard<std::mutex> Guard(MapMutex);

	SAssetPrimitiveData Data;
	Data.Name = InPath.stem().string();
	Data.Path = InPath;

	/** Open the asset */
	ZE::FileSystem::CIFileStream Stream(InPath, ZE::FileSystem::EFileReadFlagBits::Binary |
		ZE::FileSystem::EFileReadFlagBits::End);
	if (!Stream)
	{
		ZE::Logger::Error("Failed to open asset {}", InPath.string());
		return;
	}

	/** Get size */
	Data.Size = Stream.tellg();
	Stream.seekg(0, std::ios::beg);

	/** Parse the header */
	SAssetHeader Header;
	ZE::Serialization::CIBinaryArchive Ar(Stream);
	Ar <=> Header;

	if (!Header.IsValid())
	{
		ZE::Logger::Error("Invalid asset {} (bad header)", InPath.string());
		return;
	}

	Data.EngineVer = Header.EngineVer;
	Data.Class = ze::reflection::Class::get_by_name(Header.ClassName.c_str());

	PathTree.Add(InPath);
	DataMap.insert({ InPath, Data });

	ZE::Logger::Verbose("Registered asset {} ({})", InPath.string(),
		Header.ClassName);

	OnAssetRegistered.Broadcast(Data);
}

bool IsRegistered(const std::filesystem::path& InPath)
{
	return PathTree.HasPath(InPath);
}

bool IsValidAsset(const std::filesystem::path& InPath)
{
	return InPath.extension().string() == ".zasset";
}

void ScanInternal(const std::filesystem::path& InPath)
{
	ZE::FileSystem::IterateDirectories(InPath,
		[&](const ZE::FileSystem::SDirectoryEntry& InEntry)
		{
			if (ZE::FileSystem::IsDirectory(InPath / InEntry.Path) ||
				!IsValidAsset(InEntry.Path) || 
				IsRegistered(InEntry.Path))
				return;

			RegisterAsset(InPath / InEntry.Path);
		}, ZE::FileSystem::EIterateDirectoriesFlagBits::Recursive);

	OnAssetScanCompleted.Broadcast();
}

void Scan(const std::filesystem::path& InPath, const EAssetScanMode& InScanMode)
{
	switch (InScanMode)
	{
	case EAssetScanMode::Async:
		ZE::Async(
			[InPath](const ZE::JobSystem::SJob& InJob)
			{
				ScanInternal(InPath);
			});
		break;
	case EAssetScanMode::Sync:
		ScanInternal(InPath);
		break;
	}
}

std::vector<SAssetPrimitiveData> GetAssets(const std::filesystem::path& InDirectory)
{
	auto Childs = PathTree.GetChilds(InDirectory, true);
	
	std::vector<SAssetPrimitiveData> Assets;
	Assets.reserve(Childs.size());

	for (const auto& Child : Childs)
	{
		auto It = DataMap.find(InDirectory / Child);

		if (It != DataMap.end())
			Assets.emplace_back(It->second);
	}

	return Assets;
}

std::optional<SAssetPrimitiveData> GetAssetPrimitiveData(const std::filesystem::path& InPath)
{
	auto Data = DataMap.find(InPath);
	if (Data != DataMap.end())
		return Data->second;

	return std::nullopt;
}

std::vector<std::filesystem::path> GetSubDirectories(const std::filesystem::path& InRoot)
{
	return PathTree.GetChilds(InRoot, false);
}

TOnAssetRegistered& GetOnAssetRegistered() { return OnAssetRegistered; }
TOnAssetScanCompleted& GetOnAssetScanCompleted() { return OnAssetScanCompleted; }

}