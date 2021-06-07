#pragma once

#include "EngineCore.h"

namespace ze::gfx
{

/**
 * Describe all possible shader format a shader can be
 * This is the result of a backend shader language (ze::gfx::BackendShaderLanguage) + backend shader model (ze::gfx::BackendShaderModel)
 */
enum class ShaderFormat
{
	SPIRV_SM_6_0,
	DXIL_SM_6_0,

	SPIRV_SM_6_5,
	DXIL_SM_6_5,
};

}