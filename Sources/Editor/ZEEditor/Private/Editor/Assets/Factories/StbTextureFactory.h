#pragma once

#include "Editor/Assets/AssetFactory.h"
#include "StbTextureFactory.gen.h"

namespace ZE::Editor
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

	TOwnerPtr<CAsset> CreateFromStream(std::istream& InStream) override;
};

}