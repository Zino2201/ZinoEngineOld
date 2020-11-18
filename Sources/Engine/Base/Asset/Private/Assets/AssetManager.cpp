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

ZE_DEFINE_MODULE(ze::module::DefaultModule, Asset);

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
#if 0
	/** Try returning an existing instance */
	auto CachedAsset = CachedAssets.find(InPath);
	if (CachedAsset != CachedAssets.end() && 
		!CachedAsset->second.expired())
		return CachedAsset->second.lock();

	/** Instantiate the asset */
	std::optional<ze::assetdatabase::AssetPrimitiveData> Data 
		= ze::assetdatabase::get_asset_primitive_data(InPath);
	if (!Data)
	{
		ze::logger::error("Can't load asset {}: Not registered in asset database, may be an invalid asset", 
			InPath.string());
		return nullptr;
	}

	/** Check if asset class is valid */
	if(!Data->Class)
	{
		ze::logger::error("Can't load asset {}: Asset class invalid", InPath.string());
		return nullptr;
	}

	/** Instantiate a new class for this asset */
	//std::shared_ptr<CAsset> AssetInstance = std::shared_ptr<CAsset>(Data->Class->Instantiate<CAsset>());
	//AssetInstance->SetPath(InPath);
	//CachedAssets[InPath] = AssetInstance;
#endif
	return nullptr;
}

}