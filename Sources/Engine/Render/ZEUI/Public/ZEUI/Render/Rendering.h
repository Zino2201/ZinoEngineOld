#pragma once

#include "Maths/Vector.h"
#include "Maths/Color.h"
#include "Gfx/Gfx.h"

namespace ze::ui
{

/**
 * Default ZEUI Vertex
 */
struct Vertex
{
	maths::Vector2f position;
	maths::Vector2f texcoord;
	maths::Color color;

	Vertex(const maths::Vector2f& in_position,
		const maths::Vector2f& in_texcoord,
		const maths::Color& in_color) : position(in_position), texcoord(in_texcoord), color(in_color) {}

	static std::vector<gfx::VertexInputAttributeDescription> get_input_attribute_desc()
	{
		return 
		{
			gfx::VertexInputAttributeDescription(0, 0, gfx::Format::R32G32Sfloat, offsetof(Vertex, position)),
			gfx::VertexInputAttributeDescription(1, 0, gfx::Format::R32G32Sfloat, offsetof(Vertex, texcoord)),
			gfx::VertexInputAttributeDescription(2, 0, gfx::Format::R32G32B32A32Sfloat, offsetof(Vertex, color)),
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