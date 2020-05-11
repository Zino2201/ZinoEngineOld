#include "Engine/Components/TransformComponent.h"
#include "Reflection/Builders.h"

namespace ZE::Refl
{

REFL_INIT_BUILDERS_FUNC(TransformComponent)
{
	Builders::TStructBuilder<Components::STransformComponent>("STransformComponent")
		.Ctor<>();
}

}