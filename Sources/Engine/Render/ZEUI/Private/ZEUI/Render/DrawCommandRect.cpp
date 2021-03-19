#include "ZEUI/Render/DrawCommandRect.h"
#include "Gfx/Effect/EffectDatabase.h"

namespace ze::ui
{

void DrawCommandPrimitiveRect::build(const DrawCommand& command)
{
	effect = { gfx::effect_get_by_name("ZEUIBase"), {} };
	bindings = { };

	vertices = 
	{
		{ maths::Vector2f(0.0f, 0.0f) * command.size + command.position, maths::Vector2f(0, 0), color },
		{ maths::Vector2f(1.0f, 0.0f) * command.size + command.position, maths::Vector2f(1, 0), color },
		{ maths::Vector2f(1.0f, 1.0f) * command.size + command.position, maths::Vector2f(1, 1), color },
		{ maths::Vector2f(0.0f, 1.0f) * command.size + command.position, maths::Vector2f(0, 1), color },
	};
		
	indices = 
	{
		0, 1, 2,
		2, 3, 0
	};
}

}