#pragma once

#include "MathCore.h"
#include "Vector.h"
#include "Matrix.h"

namespace ze::maths
{

/**
 * An transform
 */
struct Transform
{
    Vector3d Position;
    
    ZE_FORCEINLINE Matrix4 to_world_matrix() const
    {
        return glm::translate(glm::mat4(1.0f), glm::vec3(Position.x, Position.y, Position.z));
    }
};

} /* namespace ZE::Math */