#include "Editor/IconManager.h"
#include <robin_hood.h>
#include <stb_image.h>
#include <ZEFS/Utils.h>

namespace ze::editor
{

/** Node hashmap so elements references are stable */
robin_hood::unordered_map<std::string, std::unique_ptr<Icon>> icons;

Icon* load_icon_file(const std::string& in_name, const std::string& in_file)
{
	std::vector<uint8_t> data = filesystem::read_file_to_vector(in_file, true);
	if(data.empty())
	{
		ze::logger::error("Failed to load icon from file {}! (Path invalid)");
		return nullptr;
	}

	int width, height, channels;
	stbi_uc* out_data = stbi_load_from_memory(data.data(), data.size(), &width, &height, &channels, STBI_rgb_alpha);
	if(data.empty())
	{
		ze::logger::error("Failed to load icon from file {}! (stbi_load_from_memory failed)");
		return nullptr;
	}

	auto ret = load_icon(in_name, width, height, { out_data, out_data + (width * height * 4) });
	stbi_image_free(out_data);
	return ret;
}

Icon* load_icon(const std::string& in_name, const uint32_t in_width, const uint32_t in_height, const std::span<uint8_t>& in_data)
{
	auto [tex_result, texture] = gfx::Device::get().create_texture(
		gfx::TextureInfo::make_2d_texture(
			in_width,
			in_height,
			gfx::Format::R8G8B8A8Unorm,
			1,
			gfx::TextureUsageFlagBits::Sampled),
		gfx::TextureInfo::InitialData(
			in_data,
			gfx::PipelineStageFlagBits::FragmentShader,
			gfx::TextureLayout::ShaderReadOnly,
			gfx::AccessFlagBits::ShaderRead));
	if(tex_result != gfx::Result::Success)
	{
		ze::logger::error("Failed to load icon {}! (Can't create texture: {})",
			in_name,
			tex_result);
		return nullptr;
	}

	auto [view_result, view] = gfx::Device::get().create_texture_view(
		gfx::TextureViewInfo::make_2d_view(texture,
			gfx::Format::R8G8B8A8Unorm,
			gfx::TextureSubresourceRange(gfx::TextureAspectFlagBits::Color, 0, 1, 0, 1)));

	if(view_result != gfx::Result::Success)
	{
		ze::logger::error("Failed to load icon {}! (Can't create texture view: {})",
			in_name,
			view_result);
		return nullptr;
	}

	auto it = icons.insert({ in_name, std::make_unique<Icon>(in_name, in_width, in_height, texture, view) });
	return it.first->second.get();
}

Icon* get_icon(const std::string& in_name)
{
	auto it = icons.find(in_name);
	if(it != icons.end())
		return it->second.get();

	return nullptr;
}

void free_icons()
{
	icons.clear();
}

}