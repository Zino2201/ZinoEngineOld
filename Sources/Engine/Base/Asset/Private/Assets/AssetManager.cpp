#include "Assets/AssetManager.h"
#include "Serialization/BinaryArchive.h"
#include "Assets/Asset.h"
#include <robin_hood.h>
#include "Reflection/Class.h"
#include <ostream>
#include "Module/Module.h"
#include "ZEFS/FileStream.h"
#include "AssetDatabase/AssetDatabase.h"
#include <utility>
#include "Reflection/ArchivesFwd.h"

DEFINE_MODULE(ZE::Module::CDefaultModule, Asset);

namespace std
{
	template<> struct hash<std::filesystem::path>
	{
		std::size_t operator()(const std::filesystem::path& InPath) const noexcept
		{
			return std::filesystem::hash_value(InPath);
		}
	};
}

namespace ZE::AssetManager
{

robin_hood::unordered_map<std::filesystem::path, std::weak_ptr<CAsset>> CachedAssets;

std::shared_ptr<CAsset> GetAsset(const std::filesystem::path& InPath)
{
	/** Try returning an existing instance */
	auto CachedAsset = CachedAssets.find(InPath);
	if (CachedAsset != CachedAssets.end() && 
		!CachedAsset->second.expired())
		return CachedAsset->second.lock();

	/** Instantiate the asset */
	std::optional<ZE::AssetDatabase::SAssetPrimitiveData> Data 
		= ZE::AssetDatabase::GetAssetPrimitiveData(InPath);
	if (!Data)
	{
		ZE::Logger::Error("Can't load asset {}: Not registered in asset database, may be an invalid asset", 
			InPath.string());
		return nullptr;
	}

	/** Check if asset class is valid */
	if(!Data->Class)
	{
		ZE::Logger::Error("Can't load asset {}: Asset class invalid", InPath.string());
		return nullptr;
	}

	/** Instantiate a new class for this asset */
	//std::shared_ptr<CAsset> AssetInstance = std::shared_ptr<CAsset>(Data->Class->Instantiate<CAsset>());
	//AssetInstance->SetPath(InPath);
	//CachedAssets[InPath] = AssetInstance;
	return nullptr;
}

}