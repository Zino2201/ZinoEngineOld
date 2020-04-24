#pragma once

#include "ECS.h"

/**
 * Basic components to be derived
 */
namespace ZE
{

/**
 * Component storing hierarchy data
 */
struct SHierarchyComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(SHierarchyComponent, SEntityComponent)

    uint32_t ChildrenCount;
    ECS::EntityID Parent;
    ECS::EntityID First;
    ECS::EntityID Previous;
    ECS::EntityID Next;
    ECS::EntityID Last;

    SHierarchyComponent() : ChildrenCount(0),
        Parent(ECS::GNullEntityID),
        First(ECS::GNullEntityID),
        Previous(ECS::GNullEntityID),
        Next(ECS::GNullEntityID),
        Last(ECS::GNullEntityID) {}

};

/**
 * A component storing transform data
 */
struct STransformComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(STransformComponent, SEntityComponent)

    Math::STransform Transform;
};

/**
 * A component storing velocity data
 */
struct SVelocityComponent : public ECS::SEntityComponent
{
    DECLARE_REFL_STRUCT_OR_CLASS1(SVelocityComponent, SEntityComponent)

    Math::SVector3 Velocity;
};

DECLARE_REFL_TYPE(SHierarchyComponent);
DECLARE_REFL_TYPE(STransformComponent);
DECLARE_REFL_TYPE(SVelocityComponent);

}