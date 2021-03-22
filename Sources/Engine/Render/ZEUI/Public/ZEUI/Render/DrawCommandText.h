#pragma once

#include "DrawCommand.h"
#include "Font.h"

struct hb_buffer_t;
struct hb_glyph_info_t;
struct hb_glyph_position_t;

namespace ze::ui
{

struct DrawCommandPrimitiveText final : public DrawCommandPrimitive
{
	FontInfo font;
	hb_buffer_t* text;
	size_t text_size;
	TextAlignMode align_mode;
	gfx::DeviceResourceHandle font_data;
	maths::Color color;

	DrawCommandPrimitiveText(const FontInfo& in_font,
		hb_buffer_t* in_text,
		const size_t in_text_size,
		const TextAlignMode in_align_mode,
		const gfx::DeviceResourceHandle& in_font_data,
		const maths::Color& in_color = maths::Color::white()) : font(in_font), text(in_text), text_size(in_text_size), 
		align_mode(in_align_mode), font_data(in_font_data),
		color(in_color), idx_offset(0) {}

	void build(const DrawCommand& commmand) override;
	static maths::Vector2f measure(TextAlignMode align_mode, hb_buffer_t* text, const FontInfo& in_font);
private:
	void print(hb_glyph_info_t& glyph_info, hb_glyph_position_t& glyph_pos);
private:
	size_t idx_offset;
	maths::Vector2f cursor_pos;
};

}