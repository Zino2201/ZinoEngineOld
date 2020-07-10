#pragma once

#include "EngineCore.h"

namespace ZE::Renderer
{

/**
 * Render pass flags
 */
enum class ERenderPass
{
	None = 1 << 0,
	
	BasePass = 1 << 1,

	Count = 1,
};
DECLARE_FLAG_ENUM(ERenderPass);


}