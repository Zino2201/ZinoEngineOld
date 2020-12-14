#pragma once

#include "Editor/Assets/AssetFactory.h"
#include "StbTextureFactory.gen.h"

namespace ze::editor
{

/**
 * Texture format using stb_image for importing common bitmap formats
 */
ZCLASS()
class StbTextureFactory : public AssetFactory
{
	ZE_REFL_BODY()

public:
	StbTextureFactory();

	OwnerPtr<Asset> instantiate() override;
	OwnerPtr<Asset> create_from_stream(std::istream& in_stream) override;
};

}