#pragma once

#include "EngineCore.h"
#include "AssetFactory.gen.h"

namespace ze { class Asset; }

namespace ze::editor
{

/**
 * Base abstract class for asset factories
 * Asset factories manage creating new asset instances 
 *	when the user imports a asset
 */
ZCLASS()
class AssetFactory
{
	ZE_REFL_BODY()

public:
	AssetFactory();
	virtual ~AssetFactory() = default;
	
	virtual OwnerPtr<Asset> instantiate() { return nullptr; }

	/**
	 * Create a new asset from a stream
	 * @param InStream The imported asset stream
	 */
	virtual OwnerPtr<Asset> create_from_stream(std::istream& in_stream) { return nullptr; }

	const std::string& get_name() const { return name; }
	const std::vector<std::string>& get_supported_formats() const { return supported_formats;  }
	const ze::reflection::Class* get_supported_class() const { return asset_class;  }
	const bool can_instantiated() const { return can_be_instantiated;  }
	const std::string& get_asset_file_extension() const { return asset_file_extension; }
protected:
	std::string name;
	
	/** The class that the factory represents */
	const ze::reflection::Class* asset_class;

	/** Supported assets formats */
	std::vector<std::string> supported_formats;

	/** Can be instantiated from right clicking the asset explorer ? */
	bool can_be_instantiated;

	/** Asset file extension to use, defaults to zeasset */
	std::string asset_file_extension;
};

void initialize_asset_factory_mgr();
void destroy_asset_factory_mgr();

/**
 * Get a asset factory for the specified format
 * May be nullptr if no asset factory support this format
 */
AssetFactory* get_factory_for_format(const std::string& InSupportedFormat);
const std::vector<std::unique_ptr<AssetFactory>>& get_factories();

}