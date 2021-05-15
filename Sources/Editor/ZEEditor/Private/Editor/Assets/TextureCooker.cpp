#include "Editor/Assets/TextureCooker.h"
#include "Engine/Assets/Texture.h"
#if ZE_WITH_EDITOR
#include "TextureCompressor.h"
#include "AssetDataCache/AssetDataCache.h"
#endif

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

	/** Regenerate mipmaps from source and compress them */
	{
		auto start = std::chrono::high_resolution_clock::now();
		compressed_data = texc_compress(texture->get_width(), texture->get_height(), 1,
			texture->get_uncompressed_data(), TexcPixelFormat::RGBA32Unorm, get_adequate_format(texture), miplevels);
		auto end = std::chrono::high_resolution_clock::now();
		logger::verbose("Compressed {} in {}s", uuids::to_string(texture->get_uuid()), std::chrono::duration<float>(end - start).count());
	}

	/** Store into asset cache */
	{
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
		if (in_texture->get_format() == TextureFormat::LinearRGB || in_texture->get_format() == TextureFormat::LinearRGBA)
			return gfx::Format::R8G8B8A8Unorm;
		else
			return gfx::Format::R8G8B8A8Srgb;
	/** Default compression, BC1/BC3 on PC */
	case TextureCompressionMode::Default:
	{
		if (in_texture->get_format() == TextureFormat::LinearRGBA) /** RGBA */
			return gfx::Format::Bc3UnormBlock;
		else if (in_texture->get_format() == TextureFormat::LinearRGB) /** RGB */
			return gfx::Format::Bc1RgbUnormBlock;
		else if (in_texture->get_format() == TextureFormat::sRGBA) /** sRGBA */
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