#pragma once

#include "Maths/Vector.h"
#include "Gfx/Backend.h"

namespace ze::ui
{

struct Vertex
{
	maths::Vector2f position;
	maths::Vector3f color;

	Vertex(const maths::Vector2f& in_position,
		const maths::Vector3f& in_color) : position(in_position), color(in_color) {}

	static std::vector<gfx::VertexInputAttributeDescription> get_input_attribute_desc()
	{
		return 
		{
			gfx::VertexInputAttributeDescription(0, 0, gfx::Format::R32G32Sfloat, offsetof(Vertex, position)),
			gfx::VertexInputAttributeDescription(1, 0, gfx::Format::R32G32B32Sfloat, offsetof(Vertex, color)),
		};
	}

	static std::vector<gfx::VertexInputBindingDescription> get_input_binding_desc()
	{
		return 
		{
			gfx::VertexInputBindingDescription(0, sizeof(Vertex), gfx::VertexInputRate::Vertex),
		};
	}
};

}