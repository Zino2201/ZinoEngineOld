#include "ZEUI/Render/Font.h"

namespace ze::ui
{

Font::Font(const uint32_t in_texture_width,
	const uint32_t in_texture_height,
	const std::vector<uint8_t>& in_texture_data,
	const double in_texture_pixel_range,
	const std::vector<Glyph>& in_glyphs,
	const double in_em_size,
	const double in_space_advance,
	const double in_tab_advance)
	: data(in_texture_data), width(in_texture_width), height(in_texture_height), sdf_pixel_range(in_texture_pixel_range),
	em_size(in_em_size), space_advance(in_space_advance), tab_advance(in_tab_advance)
{
	texture = gfx::Device::get().create_texture(
		gfx::TextureInfo::make_2d_texture(in_texture_width, in_texture_height, gfx::Format::R8G8B8A8Unorm, 1, gfx::TextureUsageFlagBits::Sampled),
		gfx::TextureInfo::InitialData(data,
			gfx::PipelineStageFlagBits::FragmentShader,
			gfx::TextureLayout::ShaderReadOnly, 
			gfx::AccessFlagBits::ShaderRead)).second;

	texture_view = gfx::Device::get().create_texture_view(
		gfx::TextureViewInfo::make_2d_view(*texture, gfx::Format::R8G8B8A8Unorm, 
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, 0, 1, 0, 1))).second;

	for(const auto& glyph : in_glyphs)
		glyphs.insert({ glyph.character, glyph });
}

}