#include "Engine/Components/StaticMeshComponent.h"
#include "Reflection/Builders.h"

namespace ZE
{

namespace Refl
{

REFL_INIT_BUILDERS_FUNC(StaticMeshComponent)
{
	Builders::TStructBuilder<Components::SStaticMeshComponent>("SStaticMeshComponent")
		.Ctor<>();
}

}

}