#include "EngineCore.h"
#include "Gfx/GfxCore.h"

namespace ze
{

/**
 * Format of the pixels
 */
enum class TexcPixelFormat
{
	RGB32Unorm,
	RGBA32Unorm,
};
	
/**
 * Compress a single RGB(A) 2d/3d texture mipmap to a chain of compressed mipmaps 
 */
TEXTURECOMPRESSOR_API std::vector<std::vector<uint8_t>> texc_compress(const uint32_t in_width,
	const uint32_t in_height,
	const uint32_t in_depth,
	const std::vector<uint8_t>& in_uncompressed_data,
	const TexcPixelFormat in_src_pixel_format,
	const gfx::Format& in_target_format,
	const uint32_t in_mip_levels = 1);

}