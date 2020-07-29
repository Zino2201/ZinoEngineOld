#pragma once

#include "EngineCore.h"

namespace ZE::Renderer
{

/**
 * Render pass flags
 */
enum class ERenderPassFlagBits
{
	None = 0,
	
	BasePass = 1 << 0,

	Count = 1,
};
ENABLE_FLAG_ENUMS(ERenderPassFlagBits, ERenderPassFlags);



}