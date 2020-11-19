#pragma once

#include "Assets/Asset.h"
#include "Serialization/Types/Vector.h"
#include "Reflection/Enum.h"
#include "Texture.gen.h"

namespace ze
{

/**
 * The type of the texture
 */
ZENUM()
enum class TextureType
{
	Tex1D,
	Tex2D,
	Tex3D,
	Cubemap, 
};

ZENUM()
enum class TextureFilter
{
	Linear,
	Nearest
};

/**
 * Determines the final texture format used and parameters used
 */
ZENUM()
enum class TextureCompressionMode
{
	/** Normal, on PC this will use DXT1/5 */
	Normal,

	/** For UIs only, this will disable mipmapping */
	UI,
};

/**
 * Format the texture is stored in
 */
ZENUM()
enum class TextureFormat
{
	DXT1,
	DXT5,
	R8G8B8A8
};

/**
 * A texture
 * On editor builds, this is stored in RGBA format, the platform-specific compressed
 *	texture is stored in the AssetCache
 * On non-editor builds, the format is platform-specific, on PCs it is commonly DXT1/5 for non-UI textures
 */
ZCLASS()
class ENGINE_API Texture : public Asset
{
	ZE_REFL_BODY() 

public:
	Texture() = default;
	
	Texture(const TextureType in_type,
		const TextureFilter in_filter,
		const TextureCompressionMode in_compression_mode,
		const TextureFormat in_format,
		const uint32_t in_width,
		const uint32_t in_height,
		const uint32_t in_depth,
		const std::vector<uint8_t>& in_data) 
		: type(in_type), filter(in_filter), compression_mode(in_compression_mode), format(in_format), 
		width(in_width), height(in_height), depth(in_depth), data(in_data) {}
	
	template<typename ArchiveType>
	void serialize(ArchiveType& in_archive)
	{
		in_archive <=> type;
		in_archive <=> filter;
		in_archive <=> compression_mode;
		in_archive <=> format;
		in_archive <=> width;
		in_archive <=> height;
		in_archive <=> depth;
		in_archive <=> data;
	}
private:
	ZPROPERTY(Visible, Serializable)
	TextureType type;

	ZPROPERTY(Visible, Serializable)
	TextureFilter filter;
		
	ZPROPERTY(Visible, Serializable)
	TextureCompressionMode compression_mode;
	
	ZPROPERTY(Visible, Serializable)
	TextureFormat format;

	ZPROPERTY(Visible)
	uint32_t width;
	
	ZPROPERTY(Visible)
	uint32_t height;
	
	ZPROPERTY(Visible)
	uint32_t depth;

	ZPROPERTY()
	std::vector<uint8_t> data;
};
}