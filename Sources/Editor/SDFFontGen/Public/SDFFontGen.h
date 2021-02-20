#pragma once

#include "EngineCore.h"

namespace ze::editor
{

/**
 * Generate a signed distance field font texture from the specified font using FreeType
 * \param in_font_data Raw font data (TTF, OTF, or other supported formats by FreeType)
 * \param in_base_font_size The base font size
 * \param in_spread Spread of the glyphs in the final texture
 * \return A 8-bit R grayscale texture containing the distance field data
 */
std::vector<uint8_t> generate_sdf_texture_from_font(const std::vector<uint8_t>& in_font_data,
	const uint32_t in_base_font_size,
	const uint32_t spread);

}