#include "ZEUI/Render/DrawCommandRect.h"

namespace ze::ui
{

void DrawCommandPrimitiveRect::build(const DrawCommand& command)
{
	vertices = 
	{
		{ maths::Vector2f(0.0f, 0.0f) * command.size + command.position, maths::Vector3f(1, 0, 0), maths::Vector2f(0, 0) },
		{ maths::Vector2f(1.0f, 0.0f) * command.size + command.position, maths::Vector3f(0, 1, 0), maths::Vector2f(1, 0) },
		{ maths::Vector2f(1.0f, 1.0f) * command.size + command.position, maths::Vector3f(0, 0, 1), maths::Vector2f(1, 1) },
		{ maths::Vector2f(0.0f, 1.0f) * command.size + command.position, maths::Vector3f(1, 1, 0), maths::Vector2f(0, 1) },
	};
		
	indices = 
	{
		0, 1, 2,
		2, 3, 0
	};
}

}