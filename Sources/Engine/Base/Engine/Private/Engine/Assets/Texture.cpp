#include "Engine/Assets/Texture.h"
#if ZE_WITH_EDITOR
#include "AssetDataCache/AssetDataCache.h"
#include "TextureCompressor.h"
#include "Editor/NotificationSystem.h"
#endif

namespace ze
{

#if ZE_WITH_EDITOR
std::vector<uint8_t> Texture::load_data_cache(const uint32_t in_mip_level)
{
	if(compression_mode == TextureCompressionMode::None)
	{
		return mipmaps[in_mip_level].data;
	}

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
	else
	{
		auto start = std::chrono::high_resolution_clock::now();

		std::vector<uint8_t> data_to_cache = 
			texc_compress(mipmaps[in_mip_level].width, mipmaps[in_mip_level].height, depth, 
				mipmaps[in_mip_level].data, TexcPixelFormat::RGBA32Unorm, gfx_format);

		auto end = std::chrono::high_resolution_clock::now();
		
		ze::logger::verbose("Compressed {} in {} sec", get_path().string(), 
			std::chrono::duration<float>(end - start).count());

		assetdatacache::cache("Texture", key, data_to_cache);
		
		return data_to_cache;
	}
}
#endif

void Texture::generate_mipmaps(const std::vector<uint8_t>& in_data)
{
	if(in_data.empty())
		return;

	uint32_t miplevels = 1;
	if(use_mipmaps)
		miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

	mipmaps.clear();
	mipmaps.reserve(miplevels);

	using namespace gfx;

	/** Mip0 texture */
	TextureInfo tex_create_info;
	tex_create_info.create_info.format = gfx::Format::R8G8B8A8Unorm;
	tex_create_info.create_info.mem_usage = MemoryUsage::GpuOnly;
	tex_create_info.create_info.array_layers = 1;
	tex_create_info.create_info.mip_levels = miplevels;
	tex_create_info.create_info.width = width;
	tex_create_info.create_info.height = height;
	tex_create_info.create_info.depth = depth;
	switch(type)
	{
	case ze::TextureType::Tex1D:
		tex_create_info.create_info.type = gfx::TextureType::Tex1D;
		break;

	case ze::TextureType::Tex2D:
		tex_create_info.create_info.type = gfx::TextureType::Tex2D;
		break;

	case ze::TextureType::Tex3D:
		tex_create_info.create_info.type = gfx::TextureType::Tex3D;
		break;
	}
		
	auto [result, handle] = Device::get().create_texture(tex_create_info);
	gfx::UniqueTexture texture(handle);

	CommandList* list = Device::get().allocate_cmd_list(gfx::CommandListType::Gfx);
	/** First, copy data to mipmap 0 in order to generate the rest */
	{
		gfx::UniqueBuffer staging_buf(gfx::Device::get().create_buffer(
			gfx::BufferInfo::make_staging_buffer(in_data.size()), { (uint8_t*) in_data.data(), in_data.size() }).second);

		/** Transition all subresources to TransferWrite, 
		 * copy our base data to mipmap 0 and transition to TransferSrc for blitting */
		list->texture_barrier(*texture,
			gfx::PipelineStageFlagBits::TopOfPipe,
			gfx::AccessFlags(),
			gfx::TextureLayout::Undefined,
			gfx::PipelineStageFlagBits::Transfer,
			gfx::AccessFlagBits::TransferWrite,
			gfx::TextureLayout::TransferDst,
			TextureSubresourceRange(TextureAspectFlagBits::Color, 0, mipmaps.size() - 1, 0, 1));

		list->copy_buffer_to_texture(staging_buf.get(),
			*texture,
			gfx::BufferTextureCopyRegion(0,
				gfx::TextureSubresourceLayers(TextureAspectFlagBits::Color, 0, 0, 1),
				gfx::Offset3D(),
				gfx::Extent3D(width, height, depth)));

		list->texture_barrier(*texture,
			gfx::PipelineStageFlagBits::Transfer,
			gfx::AccessFlagBits::TransferWrite,
			gfx::TextureLayout::TransferDst,
			gfx::PipelineStageFlagBits::Transfer,
			gfx::AccessFlagBits::TransferRead,
			gfx::TextureLayout::TransferSrc,
			TextureSubresourceRange(TextureAspectFlagBits::Color, 0, 1, 0, 1));

		mipmaps.emplace_back(width, height, depth, in_data);
	}

	//uint32_t mip_width = width;
	//uint32_t mip_height = height;
	//uint32_t mip_depth = 1;
	//std::vector<UniqueBuffer> staging_buffers;
	//staging_buffers.reserve(miplevels);
	//for(uint32_t miplevel = 1; miplevel < miplevels; ++miplevel)
	//{
	//	list.enqueue<hlcs::CommandBlitTexture>(
	//		*texture,
	//		TextureLayout::TransferSrc,
	//		*texture,
	//		TextureLayout::TransferDst,
	//		std::vector<TextureBlitRegion>(
	//		{ 
	//			TextureBlitRegion(
	//				TextureSubresourceLayers(TextureAspectFlagBits::Color, miplevel - 1, 0, 1),
	//				{
	//					Offset3D(0),
	//					Offset3D(mip_width, mip_height, mip_depth),
	//				},
	//				TextureSubresourceLayers(TextureAspectFlagBits::Color, miplevel, 0, 1),
	//				{
	//					Offset3D(0),
	//					Offset3D(mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, mip_depth),
	//				}),
	//		}),
	//		Filter::Linear);

	//	list.enqueue<hlcs::CommandPipelineBarrier>(
	//		PipelineStageFlagBits::Transfer,
	//		PipelineStageFlagBits::Transfer,
	//		std::vector<TextureMemoryBarrier>({
	//			TextureMemoryBarrier(
	//				*texture,
	//				AccessFlagBits::TransferWrite,
	//				AccessFlagBits::TransferRead,
	//				TextureLayout::TransferDst,
	//				TextureLayout::TransferSrc,
	//				TextureSubresourceRange(TextureAspectFlagBits::Color, miplevel, 1, 0, 1))
	//		}));

	//	if (mip_width > 1) mip_width /= 2;
	//	if (mip_height > 1) mip_height /= 2;

	//	UniqueBuffer& buffer = staging_buffers.emplace_back(RenderBackend::get().buffer_create(BufferCreateInfo(
	//		mip_width * mip_height * mip_depth * 4, BufferUsageFlagBits::TransferDst, MemoryUsage::CpuOnly)));

	//	list.enqueue<hlcs::CommandCopyTextureToBuffer>(
	//		*texture,
	//		TextureLayout::TransferSrc,
	//		*buffer,
	//		std::vector<BufferTextureCopyRegion>
	//		{
	//			BufferTextureCopyRegion(0,
	//				TextureSubresourceLayers(TextureAspectFlagBits::Color, miplevel, 0, 1),
	//				Offset3D(),
	//				Extent3D(mip_width, mip_height, mip_depth))
	//		});

	//	mipmaps.emplace_back(mip_width, mip_height, mip_depth, in_data);
	//}

	gfx::Device::get().submit(list);

	/** Acquire subresources and copy them to CPU */
	for(uint32_t i = 1; i < mipmaps.size(); ++i)
	{
		/*mipmaps[i].data.resize(mipmaps[i].width * mipmaps[i].height * mipmaps[i].depth * 4);

		auto [map_result, map_data] = Device::get().map_buffer(*staging_buffers[i - 1]);
		ZE_ASSERTF(map_result == Result::Success, "Can't generate mipmaps: failed to map staging buffer for miplevel {} ({})", 
			i, std::to_string(map_result));
		memcpy(mipmaps[i].data.data(), map_data, mipmaps[i].data.size());
		RenderBackend::get().buffer_unmap(*staging_buffers[i - 1]);*/
	}
}

void Texture::update_resource()
{
	using namespace gfx;

	gfx_format = get_adequate_gfx_format();

	{
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
#if ZE_WITH_EDITOR
			const auto& data = load_data_cache(i);
			mipmaps[i].cached_data = data;
#else
			const auto& data = mipmaps[i].data;
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
	}

	{
		TextureViewInfo view_info;
		switch(type)
		{
		case ze::TextureType::Tex1D:
			ZE_DEBUGBREAK();
			break;
		case ze::TextureType::Tex2D:
			view_info = TextureViewInfo::make_2d_view(*texture, gfx_format, TextureSubresourceRange(
				TextureAspectFlagBits::Color, 0, 1, 0, 1));
			break;
		case ze::TextureType::Tex3D:
			ZE_DEBUGBREAK();
			break;
		}
		texture_view = Device::get().create_texture_view(view_info).second;
	}

#if !ZE_WITH_EDITOR
	/** Free data from the CPU as we don't need it anymore */
	if(!keep_in_ram)
		mipmaps.clear();
#endif
}

gfx::Format Texture::get_adequate_gfx_format() const
{
	switch(compression_mode)
	{
	case TextureCompressionMode::None:
		return gfx::Format::R8G8B8A8Unorm; /** GPUs usally don't support R8G8B8Unorm */

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