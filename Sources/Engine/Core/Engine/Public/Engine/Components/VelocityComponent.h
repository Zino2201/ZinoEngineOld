#pragma once

#include "ECS.h"

/**
 * Basic components to be derived
 */
namespace ZE
{

/**
 * A component storing velocity data
 */
struct SVelocityComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(SVelocityComponent, SEntityComponent)

    Math::SVector3 Velocity;
};

DECLARE_REFL_TYPE(SVelocityComponent);

}