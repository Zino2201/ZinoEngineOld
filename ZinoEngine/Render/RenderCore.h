#pragma once

#include "Core/EngineCore.h"

/**
 * Shader stages
 */
enum class EShaderStage
{
	Vertex,
	Fragment
};

/**
 * Max frames in flight
 */
const uint32_t g_MaxFramesInFlight = 2;