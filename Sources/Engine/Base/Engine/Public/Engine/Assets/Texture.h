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
enum class ETextureType
{
	Tex1D,
	Tex2D,
	Tex3D,
	Cubemap, 
};

ZENUM()
enum class ETextureFilter
{
	Linear,
	Nearest
};

/**
 * Determines the final texture format used and parameters used
 */
ZENUM()
enum class ETextureCompressionMode
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
enum class ETextureFormat
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
class ENGINE_API CTexture : public CAsset
{
	ZE_REFL_BODY() 

public:
	template<typename ArchiveType>
	void Serialize(ArchiveType& InArchive)
	{
		CAsset::Serialize(InArchive);

		InArchive <=> Data;
	}

	void SetTextureType(const ETextureType& InType) { Type = InType; }
	void SetFilter(const ETextureFilter& InFilter) { Filter = InFilter; }
	void SetCompression(const ETextureCompressionMode& InCompressionMode) { CompressionMode = InCompressionMode; }
	void SetFormat(const ETextureFormat& InFormat) { Format = InFormat; }
	void SetData(const std::vector<uint8_t>& InData) { Data = InData; }
private:
	ZPROPERTY(Serializable)
	ETextureType Type;

	ZPROPERTY(Serializable)
	ETextureFilter Filter;
		
	ZPROPERTY(Serializable)
	ETextureCompressionMode CompressionMode;
	
	ZPROPERTY(Serializable)
	ETextureFormat Format;
	
	ZPROPERTY()
	std::vector<uint8_t> Data;
};
}