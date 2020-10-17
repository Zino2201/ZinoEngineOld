#pragma once

#include "Editor/Assets/AssetFactory.h"
#include "StbTextureFactory.gen.h"

namespace ze::editor
{

/**
 * Texture format using stb_image for importing common bitmap formats
 */
ZCLASS()
class CStbTextureFactory : public CAssetFactory
{
	ZE_REFL_BODY()

public:
	CStbTextureFactory();

	OwnerPtr<Asset> CreateFromStream(std::istream& InStream) override;
};

}