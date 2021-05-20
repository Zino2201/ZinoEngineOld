#pragma once

#include "assets/Asset.h"
#include "serialization/types/Vector.h"
#include "reflection/Enum.h"
#include "gfx/Gfx.h"
#include "Texture.gen.h"

namespace ze
{

namespace editor { class TextureCooker; }

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
	sRGBA,

	/** HDR format */
	Hdr16,
	Hdr32,
};

ZENUM()
enum class TextureCompressionMode
{
	/** No compression */
	None,

	/** Default compression (BC1/BC3/BC6H) */
	Default,

	/** High quality (BC7 slow) */
	HighQuality,

	/** R8 */
	Grayscale,

	/** BC5 */
	NormalMap,
};

/**
 * A single mipmap
 */
struct TextureMipmap
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;

	/** Actual texture data */
	std::vector<uint8_t> data;
	
	/** View to this mipmap */
	gfx::UniqueTextureView view;

	TextureMipmap() : width(0), height(0),
		depth(0) {}

	TextureMipmap(const uint32_t in_width,
		const uint32_t in_height,
		const uint32_t in_depth,
		const std::vector<uint8_t>& in_data) : width(in_width),
		height(in_height), depth(in_depth), data(in_data) {}

	template<typename ArchiveType>
	void serialize(AssetArchive<ArchiveType>& in_archive)
	{
		in_archive <=> width;
		in_archive <=> height;
		in_archive <=> depth;

		if(in_archive.metadata.asset_format != Texture::TextureAssetFormat::TextureEditor)
			in_archive <=> data;
	}

	void clear_data()
	{
		data.clear();
	}

	const auto& get_data() const
	{
		return data;
	}
};

#if ZE_WITH_EDITOR
enum class TextureSourceDataFormat
{
	R8G8B8Unorm,
	R8G8B8A8Unorm,
	R32G32B32A32Sfloat,
};

/**
 * Contains texture source data
 */
class TextureSourceData
{
public:
	TextureSourceData() {}
	TextureSourceData(const TextureSourceDataFormat& in_format,
		const std::vector<uint8_t>& in_data) : format(in_format), data(in_data) {}

	template<typename ArchiveType>
	void serialize(AssetArchive<ArchiveType>& in_archive)
	{
		in_archive <=> format;
		in_archive <=> data;
	}

	const TextureSourceDataFormat& get_format() const { return format; }
	const std::vector<uint8_t>& get_data() const { return data; }
private:
	TextureSourceDataFormat format;
	std::vector<uint8_t> data;
};
#endif

/**
 * Platform-specific data
 * Loaded from the asset cache or directly from the asset data when cooked in non-editor 
 */
class TexturePlatformData
{
	friend class editor::TextureCooker;

public:
	TexturePlatformData() : format(gfx::Format::Undefined) {}

	/** Reload the platform data. */
	void load(Texture* in_texture);

	template<typename ArchiveType>
	void serialize(AssetArchive<ArchiveType>& in_archive)
	{
		in_archive <=> format;
		in_archive <=> mipmaps;
	}

	void clear_mipmaps_data()
	{
		for(auto& mipmap : mipmaps)
		{
			mipmap.clear_data();
		}
	}

	const TextureMipmap& get_mip(const uint32_t in_idx) const { return mipmaps[in_idx]; }
	uint32_t get_mip_count() const { return mipmaps.size(); }
	const auto& get_mipmaps() const { return mipmaps; }
	gfx::Format get_format() const { return format; }
private:
	gfx::Format format;
	std::vector<TextureMipmap> mipmaps;
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

	friend class editor::TextureCooker;

public:
	enum Version
	{
		Ver0 = 0,
	};

	enum TextureAssetFormat
	{
		TextureEditor,
		TextureCookedPC,
		TextureCookedMobile,
	};

	Texture() {}
	
#if ZE_WITH_EDITOR
	Texture(const TextureType in_type,
		const TextureFilter in_filter,
		const TextureCompressionMode in_compression_mode,
		const TextureFormat& in_format,
		const uint32_t in_width,
		const uint32_t in_height,
		const uint32_t in_depth,
		const bool in_use_mipmaps,
		const TextureSourceDataFormat& in_source_data_fmt,
		const std::vector<uint8_t>& in_data,
		const bool in_create_gpu_resources) 
		: type(in_type), filter(in_filter), compression_mode(in_compression_mode), format(in_format), 
		width(in_width), height(in_height), depth(in_depth), use_mipmaps(in_use_mipmaps), keep_in_ram(false),
		source_data(in_source_data_fmt, in_data),
		ready(false)
	{ 

	}
#endif

	template<typename ArchiveType>
	void serialize(AssetArchive<ArchiveType>& in_archive, const uint32_t& in_version)
	{
		in_archive <=> type;
		in_archive <=> filter;
		in_archive <=> compression_mode;
		in_archive <=> format;
		in_archive <=> width;
		in_archive <=> height;
		in_archive <=> depth;
		in_archive <=> platform_data;
		in_archive <=> use_mipmaps;
		in_archive <=> keep_in_ram;

#if ZE_WITH_EDITOR
		if(in_archive.is_editor)
			in_archive <=> source_data;
#endif

		if(in_archive.is_loading)
			update_resource();
	}
	 
	/**
	 * Recreate the underlying texture resource
	 * This will recook the asset if in editor
	 */
	void update_resource();

	ZE_FORCEINLINE TextureType get_type() const { return type; }
	ZE_FORCEINLINE uint32_t get_width() const { return width; }
	ZE_FORCEINLINE uint32_t get_height() const { return height; }
	ZE_FORCEINLINE TextureCompressionMode get_compression_mode() const { return compression_mode; }
	ZE_FORCEINLINE TextureFormat get_format() const { return format; }
	ZE_FORCEINLINE auto& get_platform_data() const { return platform_data; }
	ZE_FORCEINLINE bool does_use_mipmaps() const { return use_mipmaps; }
	ZE_FORCEINLINE const gfx::DeviceResourceHandle& get_texture() const { return *texture; }
	ZE_FORCEINLINE const gfx::DeviceResourceHandle& get_texture_view() const { return *texture_view; }
	ZE_FORCEINLINE bool is_ready() const { return ready; }
#if ZE_WITH_EDITOR
	ZE_FORCEINLINE const auto& get_source_data() const { return source_data; }
#endif
private:
	ZPROPERTY(Serializable)
	TextureType type;

	ZPROPERTY(Editable, Visible, Serializable, Category = "Base")
	TextureFilter filter;

	ZPROPERTY(Editable, Visible, Serializable, Category = "Compression")
	TextureCompressionMode compression_mode;

	ZPROPERTY(Editable, Visible, Serializable, Category = "Base")
	TextureFormat format;

	ZPROPERTY()
	uint32_t width;
	
	ZPROPERTY()
	uint32_t height;
	
	ZPROPERTY()
	uint32_t depth;

	ZPROPERTY(Editable, Visible, Category = "Mipmaps")
	bool use_mipmaps;

	ZPROPERTY(Editable, Visible, Category = "Misc")
	bool keep_in_ram;

#if ZE_WITH_EDITOR
	TextureSourceData source_data;
#endif
	TexturePlatformData platform_data;

	/** Is texture ready yet ? */
	std::atomic_bool ready;
	
	/** Gfx handles */
	gfx::UniqueTexture texture;
	gfx::UniqueTextureView texture_view;
};
ZE_SERL_TYPE_VERSION(Texture, Texture::Ver0);

}