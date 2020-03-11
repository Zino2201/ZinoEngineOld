#pragma once

#include "Core/EngineCore.h"

/**
 * Mesh pass types flags
 */
enum class EMeshPass : uint8_t
{
    /** Depth prepass */
    Prepass = 1 << 0,

    /** Main pass for rendering geometry */
    GeometryPass = 1 << 1,
};
DECLARE_FLAG_ENUM(EMeshPass)