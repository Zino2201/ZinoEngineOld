
#include <robin_hood.h>
#include "Reflection/Class.h"
#include "Serialization/BinaryArchive.h"
#include <ostream>
#include "Module/Module.h"

DEFINE_MODULE(ZE::Module::CDefaultModule, Asset);

#if 0
namespace ZE::AssetManager
{

/**
 * A loaded asset
 */
struct SCachedAsset
{
	/** Path of the asset */
	std::string Path;

	/** Instance of the asset */
	std::weak_ptr<CAsset> Instance;

	SCachedAsset(const std::string_view& InPath) : Path(InPath) {}
};

robin_hood::unordered_map<std::string_view, SCachedAsset> CachedAssets;

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
	std::shared_ptr<CAsset> AssetInstance = std::shared_ptr<CAsset>(Asset.Class->Instantiate<CAsset>());
	AssetInstance->SetPath(InPath);
	return AssetInstance;
}

/**
 * Verify the specified asset header
 */
bool VerifyAssetHeader(const SAssetHeader& InHeader)
{
	if (strcmp(InHeader.Id, "ZASSET") != 0)
	{
		ZE::Logger::Error("Incorrect header ID. Must be ZASSET but is {}", InHeader.Id);
		return false;
	}

	if (!InHeader.EngineVer.IsCompatibleWith(GetZEVersion()))
	{
		ZE::Logger::Error("Asset ({}) is not compatible with this version ({})", InHeader.EngineVer.ToString(), GetZEVersion().ToString());
		return false;
	}

	return true;
}

/**
 * Load the specified asset
 * @param bHeaderOnly If true, only reads the header and cache the asset
 */
std::shared_ptr<CAsset> LoadAsset(const std::string& InPath,
	const bool& bHeaderOnly)
{
	using namespace ZE::Serialization;
	using namespace ZE::FileSystem;

	CIFileStream FS(InPath, EFileReadFlagBits::Binary);
	if (!FS)
	{
		ZE::Logger::Error("Failed to load asset {}: can't open file stream", InPath);
		return nullptr;
	}

	CIBinaryArchive Ar(FS);

	SAssetHeader Header;
	Ar <=> Header;
	if (!VerifyAssetHeader(Header))
	{
		ZE::Logger::Error("Failed to load asset {}: asset header invalid", InPath);
		return nullptr;
	}

	/** Skip if header only */
	if (bHeaderOnly)
		return nullptr;
}

bool SaveAsset(const std::shared_ptr<CAsset>& InAsset)
{
	verify(InAsset);

	if (!InAsset)
		return false;

	using namespace ZE::Serialization;
	using namespace ZE::FileSystem;

	COFileStream FS(InAsset->GetPath(), EFileWriteFlagBits::ReplaceExisting |
		EFileWriteFlagBits::Binary);
	if (!FS)
		return false;
;
	COBinaryArchive Ar(FS);

	/**
	 * Write header
	 */
	Ar <=> MakeAssetHeader(InAsset->GetClass()->GetName());

	return true;
}

void UncacheAsset(const TAssetPtr<CAsset>& InAsset)
{
	verify(InAsset);

	CachedAssets.erase(InAsset->GetPath());
}

}

#endif