#pragma once

#include "Maths/Vector.h"
#include "Gfx/Backend.h"

namespace ze::ui
{

struct Vertex
{
	maths::Vector2f position;
	maths::Vector3f color;
	maths::Vector2f texcoord;

	Vertex(const maths::Vector2f& in_position,
		const maths::Vector3f& in_color,
		const maths::Vector2f& in_texcoord) : position(in_position), color(in_color),
		texcoord(in_texcoord) {}

	static std::vector<gfx::VertexInputAttributeDescription> get_input_attribute_desc()
	{
		return 
		{
			gfx::VertexInputAttributeDescription(0, 0, gfx::Format::R32G32Sfloat, offsetof(Vertex, position)),
			gfx::VertexInputAttributeDescription(1, 0, gfx::Format::R32G32B32Sfloat, offsetof(Vertex, color)),
			gfx::VertexInputAttributeDescription(2, 0, gfx::Format::R32G32Sfloat, offsetof(Vertex, texcoord)),
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