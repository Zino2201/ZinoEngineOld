#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Asset.h"
#include "FileSystem/ZFS.h"
#include <robin_hood.h>
#include "Reflection/Class.h"

namespace ZE::AssetManager
{

/**
 * A cached asset data
 */
struct SCachedAsset
{
	/** Path of the asset */
	std::string Path;

	/** Class of the asset */
	Refl::CClass* Class;

	/** Instance of the asset if instanced */
	std::weak_ptr<CAsset> Instance;

	SCachedAsset(const std::string_view& InPath) : Path(InPath), Class(nullptr) {}
};

robin_hood::unordered_map<std::string_view, SCachedAsset> CachedAssets;

uint32_t SearchAndCache_FoundAssets = 0;
void SearchAndCacheAssets_Iterator(const ZE::FileSystem::SDirectoryEntry& InEntry)
{
	/** Check if not a directory */
	if(ZE::FileSystem::IsDirectory(InEntry.Path))
		return;

	auto It = CachedAssets.find(InEntry.Path);
	if (It != CachedAssets.end())
		return;

	/** Cache asset if not present */
	CachedAssets.insert({ InEntry.Path, SCachedAsset(InEntry.Path) });
	ZE::Logger::Verbose("Cached {} ", InEntry.Path);

	SearchAndCache_FoundAssets++;
}

uint32_t SearchAndCacheAssets(const std::string_view& InPath)
{
	SearchAndCache_FoundAssets = 0;

	ZE::Logger::Verbose("Searching for assets in {}", InPath);

	if(!ZE::FileSystem::IterateDirectories(InPath, 
		&SearchAndCacheAssets_Iterator))
		ZE::Logger::Error("Failed to iterate over {}", InPath);

	ZE::Logger::Verbose("Found {} assets", SearchAndCache_FoundAssets);

	return SearchAndCache_FoundAssets;
}

void GetAssets(const std::string_view& InPath)
{
}

std::shared_ptr<CAsset> GetAsset(const std::string_view& InPath)
{
	auto It = CachedAssets.find(InPath);
	if(It == CachedAssets.end())
	{
		ZE::Logger::Error("Can't get asset {}: Asset not cached, cache it first before trying to get it", InPath);
		return nullptr;
	}

	SCachedAsset& Asset = It->second;
	
	/** Check if asset class is valid */
	if(!Asset.Class)
	{
		ZE::Logger::Error("Can't load asset {}: Asset class invalid", InPath);
		return nullptr;
	}

	/** Return current instance if it exist */
	if(!Asset.Instance.expired())
		return Asset.Instance.lock();

	/** Instantiate a new class for this asset */
	return std::shared_ptr<CAsset>(Asset.Class->Instantiate<CAsset>());
}

}