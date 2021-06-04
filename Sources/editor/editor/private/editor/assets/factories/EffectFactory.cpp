#include "EffectFactory.h"
#include "engine/assets/Effect.h"

namespace ze::editor
{

EffectFactory::EffectFactory()
{
	name = "Effect (shader)";
	asset_class = reflection::Class::get<Effect>();
	asset_file_extension = "zeeffect";
	can_be_instantiated = true;
}

OwnerPtr<Asset> EffectFactory::instantiate()
{
	return new Effect;
}

}