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
class ZEEDITOR_API AssetFactory
{
	ZE_REFL_BODY()

public:
	virtual ~AssetFactory() = default;

	/**
	 * Create a new asset from a stream
	 * @param InStream The imported asset stream
	 */
	virtual OwnerPtr<Asset> create_from_stream(std::istream& in_stream) = 0;

	const std::vector<std::string>& get_supported_formats() const { return supported_formats;  }
protected:
	/** The class that the factory represents */
	const ze::reflection::Class* asset_class;

	/** Supported assets formats */
	std::vector<std::string> supported_formats;
};

void initialize_asset_factory_mgr();
void destroy_asset_factory_mgr();

/**
 * Get a asset factory for the specified format
 * May be nullptr if no asset factory support this format
 */
ZEEDITOR_API AssetFactory* get_factory_for_format(const std::string& InSupportedFormat);

}