#include "Engine/Assets/Texture.h"
#include "Threading/JobSystem/Async.h"
#if ZE_WITH_EDITOR
#include "AssetDataCache/AssetDataCache.h"
#include "TextureCompressor.h"
#include "Editor/NotificationSystem.h"
#endif

namespace ze
{

#if ZE_WITH_EDITOR
void Texture::generate_mipmaps()
{
	ZE_CHECK(!uncompressed_data.empty());
	gfx_format = get_adequate_gfx_format();

	uint32_t miplevels = 1;
	if(use_mipmaps)
		miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

	mipmaps.clear();
	mipmaps.reserve(miplevels);

	auto start = std::chrono::high_resolution_clock::now();

	std::vector<std::vector<uint8_t>> compressed_data = texc_compress(width, height, depth, 
		uncompressed_data, TexcPixelFormat::RGBA32Unorm, gfx_format, miplevels);

	auto end = std::chrono::high_resolution_clock::now();
	ze::logger::verbose("Compressed {} in {} secs", get_path().string(), 
		std::chrono::duration<float>(end - start).count());

	uint32_t mip_width = width;
	uint32_t mip_height = height;
	uint32_t mip_depth = 1;
	for(size_t i = 0; i < miplevels; ++i)
	{
		std::string key = std::to_string(type) + "_" 
			+ path.stem().string() + "_"
			+ std::to_string(format) + "_"
			+ std::to_string(compression_mode) + "_"
			+ "Mip_" + std::to_string(i) + "_"
			+ std::to_string(mip_width) + "_"
			+ std::to_string(mip_height) + "_"
			+ std::to_string(mip_depth);
		assetdatacache::cache("Texture", key, compressed_data[i]);
		mipmaps.emplace_back(mip_width, mip_height, mip_depth, compressed_data[i]);

		if(mip_width > 1) mip_width /= 2;
		if(mip_height > 1) mip_height /= 2;
	}
}
#endif

void Texture::update_resource()
{
	using namespace gfx;

	gfx_format = get_adequate_gfx_format();
	ready = false;
	
	TextureCreateInfo tex_create_info;
	tex_create_info.mem_usage = MemoryUsage::GpuOnly;
	tex_create_info.usage_flags = TextureUsageFlagBits::Sampled | TextureUsageFlagBits::TransferDst;
	tex_create_info.array_layers = 1;
	tex_create_info.mip_levels = mipmaps.size();
	tex_create_info.width = width;
	tex_create_info.height = height;
	tex_create_info.depth = depth;
	tex_create_info.format = gfx_format;

	switch(type)
	{
	case ze::TextureType::Tex1D:
		tex_create_info.type = gfx::TextureType::Tex1D;
		break;

	case ze::TextureType::Tex2D:
		tex_create_info.type = gfx::TextureType::Tex2D;
		break;

	case ze::TextureType::Tex3D:
		tex_create_info.type = gfx::TextureType::Tex3D;
		break;
	}
		
	auto [result, handle] = Device::get().create_texture(gfx::TextureInfo(tex_create_info));
	texture = handle;

	CommandList* list = Device::get().allocate_cmd_list(CommandListType::Gfx);
	list->texture_barrier(
		*texture,
		PipelineStageFlagBits::TopOfPipe,
		AccessFlags(),
		TextureLayout::Undefined,
		PipelineStageFlagBits::Transfer,
		AccessFlagBits::TransferWrite,
		TextureLayout::TransferDst,
		TextureSubresourceRange(TextureAspectFlagBits::Color, 0, mipmaps.size(), 0, 1));
	
	/** Transition all subresources to TransferDst */
	for(size_t i = 0; i < mipmaps.size(); ++i)
	{
		auto data = get_mipmap_data(i);
#if ZE_WITH_EDITOR
		/** Get mipmap data, if it's null, regenerate all mipmaps */
		if(data.empty())
			generate_mipmaps();
		data = get_mipmap_data(i);
#else
		ZE_ASSERTF(!data.empty(), "No mipmap data present for mip {}!", i);
#endif
		/** Copy texture data to gpu using a staging buffer */
		UniqueBuffer staging_buf(Device::get().create_buffer(BufferInfo::make_staging_buffer(
			data.size()), { (uint8_t*) data.data(), data.size() }).second);
			
		list->copy_buffer_to_texture(staging_buf.get(),
			*texture,
			BufferTextureCopyRegion(0,
				TextureSubresourceLayers(TextureAspectFlagBits::Color, i, 0, 1),
				Offset3D(),
				Extent3D(mipmaps[i].width, mipmaps[i].height, mipmaps[i].depth)));
	}
			
	/** Transfer all to ShaderReadOnly */
	list->texture_barrier(
		*texture,
		PipelineStageFlagBits::Transfer,
		AccessFlagBits::TransferWrite,
		TextureLayout::TransferDst,
		PipelineStageFlagBits::FragmentShader,
		AccessFlagBits::ShaderRead,
		TextureLayout::ShaderReadOnly,
		TextureSubresourceRange(TextureAspectFlagBits::Color, 0, mipmaps.size(), 0, 1));
	
	Device::get().submit(list);

	TextureViewInfo view_info;
	switch(type)
	{
	case ze::TextureType::Tex1D:
		ZE_DEBUGBREAK();
		break;
	case ze::TextureType::Tex2D:
		view_info = TextureViewInfo::make_2d_view(*texture, gfx_format, TextureSubresourceRange(
			TextureAspectFlagBits::Color, 0, mipmaps.size(), 0, 1));
		break;
	case ze::TextureType::Tex3D:
		ZE_DEBUGBREAK();
		break;
	}

	texture_view = Device::get().create_texture_view(view_info).second;

	/** Create texture views for each mipmaps */
	for(size_t mip = 0; mip < mipmaps.size(); ++mip)
	{
		view_info.create_info.subresource_range.base_mip_level = mip;
		view_info.create_info.subresource_range.level_count = 1;
		mipmaps[mip].view = Device::get().create_texture_view(view_info).second;
	}

#if !ZE_WITH_EDITOR
	/** Free data from the CPU as we don't need it anymore */
	if(!keep_in_ram)
		mipmaps.clear();
#endif

	ready = true;
}

std::vector<uint8_t> Texture::get_mipmap_data(const uint32_t in_mip_level)
{
#if ZE_WITH_EDITOR
		std::string key = std::to_string(type) + "_" 
		+ path.stem().string() + "_"
		+ std::to_string(format) + "_"
		+ std::to_string(compression_mode) + "_"
		+ "Mip_" + std::to_string(in_mip_level) + "_"
		+ std::to_string(mipmaps[in_mip_level].width) + "_"
		+ std::to_string(mipmaps[in_mip_level].height) + "_"
		+ std::to_string(mipmaps[in_mip_level].depth);

	if(assetdatacache::has_key(key))
	{
		return assetdatacache::get_sync(key);
	}
#endif

	return mipmaps[in_mip_level].data;
}

gfx::Format Texture::get_adequate_gfx_format() const
{
	switch(compression_mode)
	{
	case TextureCompressionMode::None:
		if(format == TextureFormat::LinearRGB || format == TextureFormat::LinearRGBA)
			return gfx::Format::R8G8B8A8Unorm;
		else
			return gfx::Format::R8G8B8A8Srgb;

	/** Default compression, BC1/BC3 */
	case TextureCompressionMode::Default:
	{
		if(format == TextureFormat::LinearRGBA) /** RGBA */
			return gfx::Format::Bc3UnormBlock;
		else if(format == TextureFormat::LinearRGB) /** RGB */
			return gfx::Format::Bc1RgbUnormBlock;
		else if(format == TextureFormat::sRGBA) /** sRGBA */
			return gfx::Format::Bc3SrgbBlock;
		else /** sRGB */
			return gfx::Format::Bc1RgbSrgbBlock;
	}
	case TextureCompressionMode::Grayscale:
		ZE_DEBUGBREAK();
		break;
	case TextureCompressionMode::NormalMap:
		return gfx::Format::Bc5UnormBlock;
	case TextureCompressionMode::HighQuality:
		return gfx::Format::Bc7UnormBlock;
	case TextureCompressionMode::HDR:
		return gfx::Format::Bc6HUfloatBlock;
	}
}

}