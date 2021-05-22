#pragma once

#include "editor/assets/AssetFactory.h"
#include "EffectFactory.gen.h"

namespace ze::editor
{

ZCLASS()
class EffectFactory : public AssetFactory
{
	ZE_REFL_BODY()

public:
	EffectFactory();

	OwnerPtr<Asset> instantiate() override;
};

}