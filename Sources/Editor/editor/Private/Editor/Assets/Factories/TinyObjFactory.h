#pragma once

#include "editor/assets/AssetFactory.h"
#include "TinyObjFactory.gen.h"

namespace ze::editor
{

ZCLASS()
class TinyObjFactory : public AssetFactory
{
	ZE_REFL_BODY()

public:
	TinyObjFactory();

	OwnerPtr<Asset> create_from_stream(std::istream& in_stream) override;
};

}