#pragma once

#include "DrawCommand.h"
#include "Font.h"

struct hb_buffer_t;

namespace ze::ui
{

struct DrawCommandPrimitiveText final : public DrawCommandPrimitive
{
	FontInfo font;
	hb_buffer_t* text;
	size_t text_size;
	gfx::DeviceResourceHandle font_data;

	DrawCommandPrimitiveText(const FontInfo& in_font,
		hb_buffer_t* in_text,
		const size_t in_text_size,
		const gfx::DeviceResourceHandle& in_font_data) : font(in_font), text(in_text), text_size(in_text_size), font_data(in_font_data) {}

	void build(const DrawCommand& commmand) override;
};

}