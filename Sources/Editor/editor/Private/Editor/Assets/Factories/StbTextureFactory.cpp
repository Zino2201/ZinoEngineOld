#include "StbTextureFactory.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ios>
#include <istream>
#include "engine/assets/Texture.h"

namespace ze::editor
{

StbTextureFactory::StbTextureFactory()
{
	supported_formats = { "png", "jpg", "bmp", "tga", "jpeg", "hdr" };
	asset_class = reflection::Class::get<Texture>();
	asset_file_extension = "zetexture";
}

OwnerPtr<Asset> StbTextureFactory::instantiate()
{
	return nullptr;
}

OwnerPtr<Asset> StbTextureFactory::create_from_stream(std::istream& in_stream)
{
	/** Read texture to a vector */
	in_stream.seekg(0, std::ios::end);
	int64_t Size = in_stream.tellg();
	in_stream.seekg(0, std::ios::beg);

	std::vector<uint8_t> array;
	array.resize(Size / sizeof(uint8_t));

	in_stream.read(reinterpret_cast<char*>(array.data()), Size);

	if(stbi_is_hdr_from_memory(array.data(), array.size()))
	{
		return load_hdr(array);
	}
	else
	{
		return load_ldr(array);
	}
}

OwnerPtr<Asset> StbTextureFactory::load_ldr(const std::vector<uint8_t>& array)
{
	int width, height, channels;
	std::unique_ptr<stbi_uc> data = std::unique_ptr<stbi_uc>(stbi_load_from_memory(array.data(), array.size(),
		&width, &height, &channels, STBI_rgb_alpha));
	if (!data)
	{
		ze::logger::error("Can't import texture: {}", stbi_failure_reason());
		return nullptr;
	}

	/** Instantiate the texture */
	OwnerPtr<Texture> texture = new Texture(
		TextureType::Tex2D,
		TextureFilter::Linear,
		TextureCompressionMode::Default,
		channels == STBI_rgb_alpha ? TextureFormat::sRGBA : TextureFormat::sRGB,
		width,
		height,
		1,
		true,
		channels == STBI_rgb_alpha ? TextureSourceDataFormat::R8G8B8Unorm : TextureSourceDataFormat::R8G8B8A8Unorm,
		std::vector<uint8_t>(reinterpret_cast<uint8_t*>(data.get()),
			data.get() + (width * height * 4)),
		false);
	return texture;
}

OwnerPtr<Asset> StbTextureFactory::load_hdr(const std::vector<uint8_t>& array)
{
	int width, height, channels;
	std::unique_ptr<uint8_t> data = std::unique_ptr<uint8_t>((uint8_t*) stbi_loadf_from_memory(array.data(), array.size(),
		&width, &height, &channels, STBI_rgb_alpha));

	if (!data)
	{
		ze::logger::error("Can't import texture: {}", stbi_failure_reason());
		return nullptr;
	}

	/** Instantiate the texture */
	OwnerPtr<Texture> texture = new Texture(
		TextureType::Tex2D,
		TextureFilter::Linear,
		TextureCompressionMode::Default,
		TextureFormat::Hdr16,
		width,
		height,
		1,
		true,
		TextureSourceDataFormat::R32G32B32A32Sfloat,
		std::vector<uint8_t>(reinterpret_cast<uint8_t*>((uint8_t*) data.get()),
			((uint8_t*) data.get()) + (width * height * 4 * sizeof(float))),
		false);
	return texture;
}

}