#pragma once

#include "Gfx/Gfx.h"
#include <optional>

namespace ze::editor
{

struct Icon
{
	std::string name;
	uint32_t width;
	uint32_t height;
	gfx::UniqueTexture texture;
	gfx::UniqueTextureView texture_view;

	Icon(const std::string& in_name,
		const uint32_t in_width,
		const uint32_t in_height,
		const gfx::DeviceResourceHandle& in_texture,
		const gfx::DeviceResourceHandle& in_texture_view) :
		name(in_name), width(in_width), height(in_height),
		texture(in_texture), texture_view(in_texture_view) {}

	Icon(const Icon&) = delete;
	void operator=(const Icon&) = delete;
};

/**
 * Load a new icon to the icon database with the given name
 * \param in_name Name of the icon
 * \param in_width Width of the icon
 * \param in_height Height of the icon
 * \param in_data RGBA32 (non-SRGB) data of the icon
 * \return The loaded icon
 */
Icon* load_icon(const std::string& in_name, 
	const uint32_t in_width, const uint32_t in_height, const std::span<uint8_t>& in_data);

/** 
 * Load the specified file into a icon using stb_image
 */
Icon* load_icon_file(const std::string& in_name, const std::string& in_file);

/**
 * Get the specified icon
 * \param in_name The name to search
 * \return The icon (maybe std::nullopt)
 */
Icon* get_icon(const std::string& in_name);

/**
 * Free all icons memory
 */
void free_icons();

}