#include "ZEUI/Render/DrawCommandRect.h"

namespace ze::ui
{

std::pair<std::vector<Vertex>, std::vector<uint32_t>> DrawCommandPrimitiveRect::get_geometry(const DrawCommand& command)
{
	std::vector<Vertex> vertices = 
	{
		{ maths::Vector2f(0.0f, 0.0f) * command.size + command.position, maths::Vector3f(1, 0, 0) },
		{ maths::Vector2f(1.0f, 0.0f) * command.size + command.position, maths::Vector3f(0, 1, 0) },
		{ maths::Vector2f(1.0f, 1.0f) * command.size + command.position, maths::Vector3f(0, 0, 1) },
		{ maths::Vector2f(0.0f, 1.0f) * command.size + command.position, maths::Vector3f(1, 1, 0) },
	};
		
	std::vector<uint32_t> indices = 
	{
		0, 1, 2,
		2, 3, 0
	};

	return { vertices, indices };
}

}