#include "assets/AssetCooker.h"
#include "PlatformMgr.h"

namespace ze
{

void AssetCooker::initialize()
{
	for (const auto& clazz : reflection::Class::get_derived_classes_from(reflection::Class::get<AssetCooker>()))
	{
		OwnerPtr<AssetCooker> factory = clazz->instantiate<AssetCooker>();
		cookers.emplace_back(factory);

		logger::info("Registered asset cooker {}", clazz->get_name());
	}
}

AssetCooker* AssetCooker::get_cooker_for(const reflection::Class* asset_class)
{
	for(const auto& cooker : cookers)
	{
		if(cooker->get_asset_class() == asset_class)
			return cooker.get();
	}

	return nullptr;
}

AssetCookingContext AssetCooker::cook_asset(Asset* asset)
{
	AssetMetadata meta = asset->get_metadata();
	AssetCookingContext ctx(asset, get_running_platform(), meta);

	if (AssetCooker* cooker = get_cooker_for(asset->get_class()))
	{
		cooker->cook(ctx);
	}

	return ctx;
}

}