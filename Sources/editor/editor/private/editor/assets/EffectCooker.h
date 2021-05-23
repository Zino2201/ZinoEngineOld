#pragma once

#include "assets/AssetCooker.h"
#include "EffectCooker.gen.h"

namespace ze::editor
{

ZCLASS()
class EffectCooker : public AssetCooker
{
	ZE_REFL_BODY()

public:
	EffectCooker();

	void cook(AssetCookingContext& in_context) override;
};

}