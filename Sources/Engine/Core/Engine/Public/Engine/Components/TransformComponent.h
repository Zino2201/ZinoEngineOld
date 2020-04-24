#pragma once

#include "EngineCore.h"
#include "ECS.h"
#include "Reflection/Reflection.h"

namespace ZE::Components
{

/**
 * A component storing transform data
 */
struct STransformComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(STransformComponent, SEntityComponent)

    Math::STransform Transform;
};
DECLARE_REFL_TYPE(STransformComponent);

}