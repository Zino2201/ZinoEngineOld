#pragma once

#include "EngineCore.h"
#include "AssetFactory.gen.h"

namespace ZE { class CAsset; }

namespace ZE::Editor
{

/**
 * Base abstract class for asset factories
 * Asset factories manage creating new asset instances 
 *	when the user imports a asset
 */
ZCLASS()
class ZEEDITOR_API CAssetFactory
{
	ZE_REFL_BODY()

public:
	virtual ~CAssetFactory() = default;

	/**
	 * Create a new asset from a stream
	 * @param InStream The imported asset stream
	 */
	virtual TOwnerPtr<CAsset> CreateFromStream(std::istream& InStream) = 0;

	const std::vector<std::string>& GetSupportedFormats() const { return SupportedFormats;  }
protected:
	/** The class that the factory represents */
	const ze::reflection::Class* AssetClass;

	/** Supported assets formats */
	std::vector<std::string> SupportedFormats;
};

void InitializeAssetFactoryMgr();
void ClearAssetFactoryMgr();

/**
 * Get a asset factory for the specified format
 * May be nullptr if no asset factory support this format
 */
ZEEDITOR_API CAssetFactory* GetFactoryForFormat(const std::string& InSupportedFormat);

}