#pragma once

#include "EngineCore.h"
#include <robin_hood.h>
#include "Gfx/Gfx.h"

namespace ze::ui
{

/**
 * A TypeFace is a collection of fonts
 * E.g: Roboto is a TypeFace that contains Bold, Italic, ... fonts
 */
class TypeFace
{
public:

private:
	//robin_hood::unordered_map<std::string, Font*> fonts;
};

/**
 * A font is a variation of a TypeFace
 * Fonts in ZinoEngine are distance-field fonts
 * E.g: Roboto Bold is a Font, variation of Roboto
 */
class Font
{
public:
	struct Glyph
	{
		uint8_t character;
		double advance;
		struct Bounds
		{
			double l, b, r, t;
		} bounds;
		maths::Rect2D atlas_rect;

		Glyph(const uint8_t in_character,
			const double in_advance,
			const Bounds in_bounds,
			const maths::Rect2D in_atlas_rect) : character(in_character),
			advance(in_advance), bounds(in_bounds), atlas_rect(in_atlas_rect) {}
	};

	Font(const uint32_t in_texture_width,
		const uint32_t in_texture_height,
		const std::vector<uint8_t>& in_texture_data,
		const double in_texture_pixel_range,
		const std::vector<Glyph>& in_glyphs,
		const double in_em_size,
		const double in_space_advance,
		const double in_tab_advance);

	const std::vector<uint8_t>& get_data() const { return data; }
	const gfx::DeviceResourceHandle& get_texture() const { return *texture; }
	const gfx::DeviceResourceHandle& get_texture_view() const { return *texture_view; }
	double get_sdf_pixel_range() const { return sdf_pixel_range; }
	uint32_t get_width() const { return width; }
	uint32_t get_height() const { return height; }
	const Glyph& get_glyph(const uint8_t in_character) const { return glyphs.find(in_character)->second; }
	bool has_glyph(const uint8_t in_character) const { return glyphs.count(in_character); }
	double get_em_size() const { return em_size; }
	double get_space_advance() const { return space_advance; }
	double get_tab_advance() const { return tab_advance; }
private:
	std::vector<uint8_t> data;
	uint32_t width;
	uint32_t height;
	double em_size;
	double sdf_pixel_range;
	double space_advance;
	double tab_advance;
	gfx::UniqueTexture texture;
	gfx::UniqueTextureView texture_view;
	robin_hood::unordered_map<uint8_t, Glyph> glyphs;
};


/**
 * Interface for classes that provide a TypeFace
 * Used so TypeFaces can be stores in Assets
 */
class TypeFaceProvider
{
public:
	virtual const TypeFace& get_type_face() const = 0;
};

/**
 * Describe how to display a font
 */
struct FontInfo
{
	Font* font;
	uint32_t size;

	FontInfo() : font(nullptr), size(0) {}
	FontInfo(Font* in_font, const uint32_t& in_size) : font(in_font), size(in_size) {}
};

}