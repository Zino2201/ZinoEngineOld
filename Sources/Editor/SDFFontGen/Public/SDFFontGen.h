#pragma once

#include "EngineCore.h"
#include "Maths/Rect.h"

namespace ze::editor
{

struct MSDFGlyph
{
	uint8_t character;
	double advance;
	struct Bounds
	{
		double l, b, r, t;
	} bounds;
	maths::Rect2D atlas_rect;

	MSDFGlyph(const uint8_t in_character,
		const double in_advance,
		const Bounds in_bounds,
		const maths::Rect2D in_atlas_rect) : character(in_character),
		advance(in_advance), bounds(in_bounds), atlas_rect(in_atlas_rect){}
};

struct MSDFFontData
{
	uint32_t width;
	uint32_t height;
	std::vector<uint8_t> raw_data;
	std::vector<MSDFGlyph> glyphs;
	double em_size;
	double space_advance;
	double tab_advance;
};

/**
 * Generate a multi-channel signed distance field font texture from the specified font using FreeType
 * \param in_font_data Raw font data (TTF, OTF, or other supported formats by FreeType)
 * \param in_base_font_size The base font size
 * \return A 8-bit RGB texture containing the distance field data
 */
MSDFFontData generate_msdf_texture_from_font(const std::vector<uint8_t>& in_font_data,
	const uint32_t in_base_font_size);

}