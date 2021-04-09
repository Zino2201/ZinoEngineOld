#pragma once

#include "Gfx/Gfx.h"
#include "Maths/Color.h"

namespace ze::ui
{

/**
 * A brush controls how a certain widget is draw
 */
struct Brush
{
	enum class Type
	{
		None,
		Color,
		Texture
	};

	struct None {};

	struct Color
	{
		maths::Color color;
	};

	struct Texture
	{
		gfx::DeviceResourceHandle texture_view;
		maths::Color tint;
	};

	Type type;
	std::variant<None, Color, Texture> info;

	Brush() : type(Type::None), info(None()) {}
	Brush(const Color& in_info) : 
		type(Type::Color), info(in_info) {}

	static Brush make_color(const maths::Color& in_color = maths::Color::white())
	{
		return Brush(Color{in_color}); 
	}
};

}