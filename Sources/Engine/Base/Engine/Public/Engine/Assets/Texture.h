#pragma once

#include "Assets/Asset.h"
#include "Serialization/Types/Vector.h"
#include "Reflection/Enum.h"
#include "Gfx/Gfx.h"
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
 * Texture format (the texture is actually stored in RGBA, this value is used to choose the correct compression format and final gpu format)
 */
ZENUM()
enum class TextureFormat
{
	/** Linear formats */
	LinearRGB,
	LinearRGBA,

	/** Non-linear formats (sRGB) */
	sRGB,
	sRGBA
};

ZENUM()
enum class TextureCompressionMode
{
	/** No compression */
	None,

	/** Default compression (BC1/BC3) */
	Default,

	/** High quality (BC7, slow) */
	HighQuality,

	/** R8 */
	Grayscale,

	/** BC5 */
	NormalMap,

	/** BC6H */
	HDR,
};

/**
 * A single mipmap
 */
struct TextureMipmap
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;

	/** Actual texture data, stored as RGB(A) 32 in editor */
	std::vector<uint8_t> data;
#if ZE_WITH_EDITOR
	/** Actual data used for rendering, only available in editor */
	std::vector<uint8_t> cached_data;
#endif

	TextureMipmap() : width(0), height(0),
		depth(0) {}

	TextureMipmap(const uint32_t in_width,
		const uint32_t in_height,
		const uint32_t in_depth,
		const std::vector<uint8_t>& in_data) : width(in_width),
		height(in_height), depth(in_depth), data(in_data) {}

	template<typename ArchiveType>
	void serialize(ArchiveType& in_archive)
	{
		in_archive <=> width;
		in_archive <=> height;
		in_archive <=> depth;
		in_archive <=> data;
	}

	const auto& get_data() const
	{
#if ZE_WITH_EDITOR
		return cached_data;
#else
		return data;
#endif
	}
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
	enum Version
	{
		Ver0 = 0,
	};

	Texture() {}
	
	Texture(const TextureType in_type,
		const TextureFilter in_filter,
		const TextureCompressionMode in_compression_mode,
		const TextureFormat& in_format,
		const uint32_t in_width,
		const uint32_t in_height,
		const uint32_t in_depth,
		const bool in_use_mipmaps,
		const std::vector<uint8_t>& in_data,
		const bool in_create_gpu_resources) 
		: type(in_type), filter(in_filter), compression_mode(in_compression_mode), format(in_format), 
		width(in_width), height(in_height), depth(in_depth), use_mipmaps(in_use_mipmaps), keep_in_ram(false)
	{ 
		generate_mipmaps(in_data);

		if(in_create_gpu_resources)
			update_resource(); 
	}
	
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
		in_archive <=> mipmaps;
		in_archive <=> use_mipmaps;
		in_archive <=> keep_in_ram;

		if constexpr (serialization::IsInputArchive<ArchiveType>)
			update_resource();
	}

	/**
	 * Recreate the underlying texture resource
	 */
	void update_resource();

	/**
	 * Generate the mipmaps for the specified data and store them in the texture's mipmaps using the render system	
	 */
	void generate_mipmaps(const std::vector<uint8_t>& in_data);

	ZE_FORCEINLINE TextureType get_type() const { return type; }
	ZE_FORCEINLINE uint32_t get_width() const { return width; }
	ZE_FORCEINLINE uint32_t get_height() const { return height; }
	ZE_FORCEINLINE const auto& get_mipmaps() const { return mipmaps; }
	ZE_FORCEINLINE const auto& get_mipmap(const size_t& in_level) const { return mipmaps[in_level]; }
	ZE_FORCEINLINE const gfx::Format& get_gfx_format() const { return gfx_format; }
	ZE_FORCEINLINE const gfx::DeviceResourceHandle& get_texture() const { return *texture; }
	ZE_FORCEINLINE const gfx::DeviceResourceHandle& get_texture_view() const { return *texture_view; }
private:
#if ZE_WITH_EDITOR
	std::vector<uint8_t> load_data_cache(const uint32_t in_mip_level);
#endif

	gfx::Format get_adequate_gfx_format() const;
private:
	ZPROPERTY(Serializable)
	TextureType type;

	ZPROPERTY(Editable, Visible, Serializable, Category = "Base")
	TextureFilter filter;
		
	ZPROPERTY(Editable, Visible, Serializable, Category = "Base")
	TextureCompressionMode compression_mode;

	ZPROPERTY(Editable, Visible, Serializable, Category = "Base")
	TextureFormat format;

	ZPROPERTY()
	uint32_t width;
	
	ZPROPERTY()
	uint32_t height;
	
	ZPROPERTY()
	uint32_t depth;

	std::vector<TextureMipmap> mipmaps;

	ZPROPERTY(Editable, Visible, Category = "Mipmaps")
	bool use_mipmaps;

	ZPROPERTY(Editable, Visible, Category = "Misc")
	bool keep_in_ram;

	gfx::Format gfx_format;
	gfx::UniqueTexture texture;
	gfx::UniqueTextureView texture_view;
};
ZE_SERL_TYPE_VERSION(Texture, Texture::Ver0);

}