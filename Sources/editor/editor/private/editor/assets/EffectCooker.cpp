#include "EffectCooker.h"
#include "engine/assets/Effect.h"

namespace ze::editor
{

EffectCooker::EffectCooker()
{
	asset_class = reflection::Class::get<Effect>();
}

void EffectCooker::cook(AssetCookingContext& in_context)
{
	if(in_context.platform.is_editor())
	{
	
	}
}

}