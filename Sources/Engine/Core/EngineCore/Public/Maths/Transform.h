#pragma once

#include "MathCore.h"
#include "Vector.h"
#include "Matrix.h"

namespace ZE::Math
{

/**
 * An transform
 */
struct STransform
{
    SVector3 Position;
    
    [[nodiscard]] SMatrix4 ToWorldMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), glm::vec3(Position.X, Position.Y, Position.Z));
    }
};

} /* namespace ZE::Math */