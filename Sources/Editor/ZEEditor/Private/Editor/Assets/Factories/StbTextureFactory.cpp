#include "StbTextureFactory.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ios>
#include <istream>
#include "Engine/Assets/Texture.h"

namespace ZE::Editor
{

CStbTextureFactory::CStbTextureFactory()
{
	SupportedFormats = { "png", "jpg" };
}

TOwnerPtr<CAsset> CStbTextureFactory::CreateFromStream(std::istream& InStream)
{
	/** Read texture to a vector */

	InStream.seekg(0, std::ios::end);
	int64_t Size = InStream.tellg();
	InStream.seekg(0, std::ios::beg);

	std::vector<uint8_t> Array;
	Array.resize(Size / sizeof(uint8_t));

	InStream.read(reinterpret_cast<char*>(Array.data()), Size);

	int Width, Height, Channels;
	std::unique_ptr<stbi_uc> Data = std::unique_ptr<stbi_uc>(stbi_load_from_memory(Array.data(), Array.size(), 
		&Width, &Height, &Channels, STBI_rgb_alpha));
	if (!Data)
	{
		ZE::Logger::Error("Can't import texture: {}", stbi_failure_reason());
		return nullptr;
	}

	/** Instantiate the texture */
	TOwnerPtr<CTexture> Texture = new CTexture;
	Texture->SetTextureType(ETextureType::Tex2D);
	Texture->SetFormat(ETextureFormat::R8G8B8A8);
	Texture->SetCompression(ETextureCompressionMode::Normal);
	Texture->SetFilter(ETextureFilter::Linear);
	Texture->SetData(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(Data.get()),
		Data.get() + (Width * Height * Channels)));
	return Texture;
}

}