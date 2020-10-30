#include "StbTextureFactory.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ios>
#include <istream>
#include "Engine/Assets/Texture.h"

namespace ze::editor
{

StbTextureFactory::StbTextureFactory()
{
	supported_formats = { "png", "jpg" };
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

	int width, height, channels;
	std::unique_ptr<stbi_uc> data = std::unique_ptr<stbi_uc>(stbi_load_from_memory(array.data(), array.size(), 
		&width, &height, &channels, STBI_rgb_alpha));
	if (!data)
	{
		ze::logger::error("Can't import texture: {}", stbi_failure_reason());
		return nullptr;
	}

	/** Instantiate the texture */
	OwnerPtr<CTexture> texture = new CTexture;
	texture->SetTextureType(ETextureType::Tex2D);
	texture->SetFormat(ETextureFormat::R8G8B8A8);
	texture->SetCompression(ETextureCompressionMode::Normal);
	texture->SetFilter(ETextureFilter::Linear);
	texture->SetData(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(data.get()),
		data.get() + (width * height * channels)));
	ZE_ASSERT(false);
	return nullptr;
}

}