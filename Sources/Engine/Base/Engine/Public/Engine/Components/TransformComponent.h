#pragma once

#include "EngineCore.h"
#include "Engine/ECS.h"
#include "Reflection/Reflection.h"
#include "TransformComponent.gen.h"

namespace ZE::Components
{

/**
 * A component storing transform data
 */
ZSTRUCT()
struct STransformComponent : public ECS::SEntityComponent
{
    ZE_REFL_BODY()

    Math::STransform Transform;
};

}