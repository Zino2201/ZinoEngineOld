#pragma once

#include "EngineCore.h"
#include <robin_hood.h>

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
 * E.g: Roboto Bold is a Font, variation of Roboto
 */
class Font
{
public:
	Font(const std::vector<uint8_t>& in_data);

	const std::vector<uint8_t>& get_data() const { return data; }
	//const gfx::UniqueTexture& get_texture() const { return texture; }
private:
	std::vector<uint8_t> data;
	//gfx::UniqueTexture texture;
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