#pragma once

#include "DrawCommand.h"
#include "Maths/Color.h"

namespace ze::ui
{

struct DrawCommandPrimitiveRect final : public DrawCommandPrimitive
{
	maths::Color color;

	DrawCommandPrimitiveRect(const maths::Color& in_color = maths::Color::white()) :
		color(in_color) {}

	void build(const DrawCommand& commmand) override;
};

}