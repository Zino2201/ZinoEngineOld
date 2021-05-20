#include "editor/assets/TextureCooker.h"
#include "engine/assets/Texture.h"
#include "TextureCompressor.h"
#include "assetdatacache/AssetDatacache.h"
#include "editor/LargeTask.h"

namespace ze::editor
{

TextureCooker::TextureCooker() 
{
	asset_class = reflection::Class::get<Texture>();
}

void TextureCooker::cook(AssetCookingContext& in_context)
{
	/** Select asset_format */
	if(in_context.platform.is_editor())
	{
		in_context.metadata.asset_format = Texture::TextureEditor;

		cook_for_editor(in_context);		
	}
	else
	{
		ZE_ASSERT(false);
	}
}

#if ZE_WITH_EDITOR
void TextureCooker::cook_for_editor(AssetCookingContext& in_context)
{
	Texture* texture = static_cast<Texture*>(in_context.asset);

	std::vector<std::vector<uint8_t>> compressed_data;
	uint32_t miplevels = 1;
	if (texture->does_use_mipmaps())
		miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texture->get_width(), texture->get_height())))) + 1;

	LargeTask parent_task("Cooking texture", 2);

	/** Regenerate mipmaps from source and compress them */
	{
		LargeTask compressing_task("Compressing texture");
		
		TexcPixelFormat pixel_format = TexcPixelFormat::RGBA32Unorm;
		switch(texture->get_source_data().get_format())
		{
		default:
			pixel_format = TexcPixelFormat::RGBA32Unorm;
			break;
		case TextureSourceDataFormat::R8G8B8Unorm:
			pixel_format = TexcPixelFormat::RGB32Unorm;
			break;
		case TextureSourceDataFormat::R32G32B32A32Sfloat:
			pixel_format = TexcPixelFormat::RGBA32Sfloat;
			break;
		}

		auto start = std::chrono::high_resolution_clock::now();
		compressing_task.work();
		compressed_data = texc_compress(texture->get_width(), 
			texture->get_height(), 
			1,
			texture->get_source_data().get_data(), 
			pixel_format, 
			get_adequate_format(texture), 
			miplevels);
		auto end = std::chrono::high_resolution_clock::now();
		logger::verbose("Compressed {} in {}s", uuids::to_string(texture->get_uuid()), std::chrono::duration<float>(end - start).count());
	}

	parent_task.work();

	/** Store into asset cache */
	{
		LargeTask caching_task("Caching", miplevels);
		auto& mipmaps = texture->platform_data.mipmaps;
		texture->platform_data.format = get_adequate_format(texture);

		mipmaps.clear();
		mipmaps.reserve(miplevels);

		uint32_t mip_width = texture->get_width();
		uint32_t mip_height = texture->get_height();
		uint32_t mip_depth = 1;
		for (size_t i = 0; i < miplevels; ++i)
		{
			std::string key = uuids::to_string(texture->get_uuid()) + "_"
				+ std::to_string(texture->get_compression_mode()) + "_"
				+ std::to_string(texture->get_format()) + "_"
				+ "Mip_" + std::to_string(i) + "_"
				+ std::to_string(mip_width) + "_"
				+ std::to_string(mip_height) + "_"
				+ std::to_string(mip_depth);

			assetdatacache::cache("TextureCooker", key, compressed_data[i]);

			mipmaps.emplace_back(mip_width, mip_height, mip_depth, compressed_data[i]);

			caching_task.work();

			if (mip_width > 1) mip_width /= 2;
			if (mip_height > 1) mip_height /= 2;
		}
	}
}
#endif

gfx::Format TextureCooker::get_adequate_format(Texture* in_texture) const
{
	switch (in_texture->get_compression_mode())
	{
	case TextureCompressionMode::None:
		if (in_texture->get_format() == TextureFormat::LinearRGB 
			|| in_texture->get_format() == TextureFormat::LinearRGBA)
			return gfx::Format::R8G8B8A8Unorm;
		else if(in_texture->get_format() == TextureFormat::sRGB ||
			in_texture->get_format() == TextureFormat::sRGBA)
			return gfx::Format::R8G8B8A8Srgb;
		else if (in_texture->get_format() == TextureFormat::Hdr16)
			return gfx::Format::R16G16B16A16Sfloat;
		else if (in_texture->get_format() == TextureFormat::Hdr32)
			return gfx::Format::R32G32B32A32Sfloat;
	/** Default compression, BC1/BC3 on PC */
	case TextureCompressionMode::Default:
	{
		if (in_texture->get_format() == TextureFormat::LinearRGBA) /** RGBA */
			return gfx::Format::Bc3UnormBlock;
		else if (in_texture->get_format() == TextureFormat::LinearRGB) /** RGB */
			return gfx::Format::Bc1RgbUnormBlock;
		else if (in_texture->get_format() == TextureFormat::sRGBA) /** sRGBA */
			return gfx::Format::Bc3SrgbBlock;
		else if (in_texture->get_format() == TextureFormat::Hdr16)
			return gfx::Format::Bc6HSfloatBlock;
		else if (in_texture->get_format() == TextureFormat::Hdr32)
			return gfx::Format::R32G32B32A32Sfloat;
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
	}
}

}