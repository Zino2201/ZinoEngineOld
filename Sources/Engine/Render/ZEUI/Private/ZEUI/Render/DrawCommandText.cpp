#include "ZEUI/Render/DrawCommandText.h"
#include "Gfx/Effect/EffectDatabase.h"
#include "Gfx/Backend.h"

namespace ze::ui
{

std::pair<std::vector<Vertex>, std::vector<uint32_t>> DrawCommandPrimitiveText::get_geometry(const DrawCommand& command)
{
	std::vector<Vertex> vertices = 
	{
		{ maths::Vector2f(0.0f, 0.0f) * command.size + command.position, maths::Vector3f(1, 0, 0), maths::Vector2f(0, 0) },
		{ maths::Vector2f(1.0f, 0.0f) * command.size + command.position, maths::Vector3f(0, 1, 0), maths::Vector2f(1, 0) },
		{ maths::Vector2f(1.0f, 1.0f) * command.size + command.position, maths::Vector3f(0, 0, 1), maths::Vector2f(1, 1) },
		{ maths::Vector2f(0.0f, 1.0f) * command.size + command.position, maths::Vector3f(1, 1, 0), maths::Vector2f(0, 1) },
	};
		
	std::vector<uint32_t> indices = 
	{
		0, 1, 2,
		2, 3, 0
	};

	return { vertices, indices };
}

gfx::EffectPermPtr DrawCommandPrimitiveText::get_effect() const
{
	return { gfx::effect_get_by_name("ZEUIDistanceFieldText"), {} };
}

std::vector<gfx::ResourceHandle> DrawCommandPrimitiveText::get_descriptor_sets() const
{
	return { };
}

}