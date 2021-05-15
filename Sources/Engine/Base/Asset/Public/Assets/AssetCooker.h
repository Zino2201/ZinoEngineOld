#pragma once

#include "EngineCore.h"
#include "Reflection/Class.h"
#include "Asset.h"
#include "Platform.h"
#include "AssetCooker.gen.h"

namespace ze
{

struct PlatformInfo;
struct AssetMetadata;

/**
 * Context for cooking a single asset instance
 */
struct AssetCookingContext
{
	/** The asset that is being cooked */
	Asset* asset;

	/** Target platform */
	const PlatformInfo& platform;

	AssetMetadata& metadata;

	/** Output cooked data */
	std::vector<uint8_t> cooked_data;

	AssetCookingContext(Asset* in_asset, const PlatformInfo& in_platform, AssetMetadata& in_metadata) : asset(in_asset),
		platform(in_platform), metadata(in_metadata) {}
};

/**
 * Base class for cookers; manage cooking of a specific asset
 * Cooking is the process of baking platform-specific resources of an asset to use for a specific platform
 * For example, textures are platform-specific assets because some platforms prefer some compression formats than other (e.g BC for Desktop, PVRTC/ASTC for mobile)
 */
ZCLASS()
class AssetCooker
{
	ZE_REFL_BODY()

public:
	AssetCooker() : asset_class(nullptr) {}
	virtual ~AssetCooker() = default;

	/** Cook the specified asset using the target context. At the end the target must contain cooked data. */
	virtual void cook(AssetCookingContext& in_context) = 0;

	static void initialize();
	static AssetCooker* get_cooker_for(const reflection::Class* asset_class);
	static AssetCookingContext cook_asset(Asset* asset);
	
	const reflection::Class* get_asset_class() const { return asset_class; }
protected:
	const reflection::Class* asset_class;
private:
	inline static std::vector<std::unique_ptr<AssetCooker>> cookers;
};

}