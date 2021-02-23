#pragma once

#include "DrawCommand.h"
#include "Font.h"

namespace ze::ui
{

struct DrawCommandPrimitiveText final : public DrawCommandPrimitive
{
	FontInfo font;
	std::string_view text;

	DrawCommandPrimitiveText(const FontInfo& in_font,
		const std::string_view& in_text) : font(in_font), text(in_text) {}

	std::pair<std::vector<Vertex>, std::vector<uint32_t>> get_geometry(const DrawCommand& commmand) override;
	gfx::EffectPermPtr get_effect() const;
	std::vector<gfx::ResourceHandle> get_descriptor_sets() const;
};

}