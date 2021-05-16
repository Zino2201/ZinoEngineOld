#pragma once

#include <cstdint>

namespace ze::gfx
{

enum class Format
{
	Undefined,
	
	/** Depth 32-bit (signed float) */
	D32Sfloat,

	/** Depth 32-bit (signed float) & stencil 8-bit (unsigned int) */
	D32SfloatS8Uint,

	/** Depth 24-bit (signed float) & stencil 8-bit (unsigned int) */
	D24UnormS8Uint,

	/** RGB 8-bit (unsigned short) */
	R8Unorm,

	/** RGB 8-bit (unsigned short) */
	R8G8B8Unorm,

	/** RGBA 8-bit (unsigned short) */
	R8G8B8A8Unorm,
	
	/** RGB 8-bit (srgb nonlinear) */
	R8G8B8A8Srgb,

	/** BGR 8-bit (unsigned short) */
    B8G8R8A8Unorm,

	/** RGBA 16-bit (signed float) */
	R16G16B16A16Sfloat,
	
	/** RG 32-bit (signed float) */
	R32G32Sfloat,

	/** RGB 32-bit (signed float) */
	R32G32B32Sfloat,
	
	/** RGBA 32-bit (signed float) */
	R32G32B32A32Sfloat,
	
	/** RGBA 32-bit (unsigned int) */
    R32G32B32A32Uint,
	
	/** R 32-bit (unsigned int) */
	R32Uint,
	
	/** R 64-bit (unsigned int) */
	R64Uint,

	/** BC1/DXT1 */
	Bc1RgbUnormBlock,
	Bc1RgbaUnormBlock,
	Bc1RgbSrgbBlock,
	Bc1RgbaSrgbBlock,
	
	/** BC3/DXT5 */
	Bc3UnormBlock,
	Bc3SrgbBlock,
	
	/** BC5 */
	Bc5UnormBlock,
	Bc5SnormBlock,

	/** BC6H */
	Bc6HUfloatBlock,
	Bc6HSfloatBlock,

	/** BC7 */
	Bc7UnormBlock,
	Bc7SrgbBlock,
};

/** Utils structures */
struct Offset3D
{
	int32_t x;
	int32_t y;
	int32_t z;

	Offset3D(const int32_t in_x = 0,
		const int32_t in_y = 0,
		const int32_t in_z = 0) : x(in_x), y(in_y), z(in_z) {}
};

struct Extent3D
{
	uint32_t x;
	uint32_t y;
	uint32_t z;

	Extent3D(const uint32_t in_x,
		const uint32_t in_y,
		const uint32_t in_z) : x(in_x), y(in_y), z(in_z) {}
};

}

namespace std
{

ZE_FORCEINLINE std::string to_string(const ze::gfx::Format& in_format)
{
	switch(in_format)
	{
	default:
	case ze::gfx::Format::Undefined:
		return "Undefined";
	case ze::gfx::Format::D24UnormS8Uint:
		return "D24UnormS8Uint";
	case ze::gfx::Format::D32Sfloat:
		return "D32Sfloat";
	case ze::gfx::Format::D32SfloatS8Uint:
		return "D32SfloatS8Uint";
	case ze::gfx::Format::B8G8R8A8Unorm:
		return "B8G8R8A8Unorm";
	case ze::gfx::Format::R8G8B8Unorm:
		return "R8G8B8Unorm";
	case ze::gfx::Format::R8G8B8A8Unorm:
		return "R8G8B8A8Unorm";
	case ze::gfx::Format::R8G8B8A8Srgb:
		return "R8G8B8A8Srgb";
	case ze::gfx::Format::R16G16B16A16Sfloat:
		return "R16G16B16A16Sfloat";
	case ze::gfx::Format::R32Uint:
		return "R32Uint";
	case ze::gfx::Format::R64Uint:
		return "R64Uint";
	case ze::gfx::Format::R32G32Sfloat:
		return "R32G32Sfloat";
	case ze::gfx::Format::R32G32B32Sfloat:
		return "R32G32B32Sfloat";
	case ze::gfx::Format::R32G32B32A32Sfloat:
		return "R32G32B32A32Sfloat";
	case ze::gfx::Format::R32G32B32A32Uint:
		return "R32G32B32A32Uint";
	case ze::gfx::Format::Bc1RgbUnormBlock:
		return "Bc1RgbUnormBlock";
	case ze::gfx::Format::Bc1RgbaUnormBlock:
		return "Bc1RgbaUnormBlock";
	case ze::gfx::Format::Bc1RgbSrgbBlock:
		return "Bc1RgbSrgbBlock";
	case ze::gfx::Format::Bc1RgbaSrgbBlock:
		return "Bc1RgbaSrgbBlock";
	case ze::gfx::Format::Bc3UnormBlock:
		return "Bc3UnormBlock";
	case ze::gfx::Format::Bc3SrgbBlock:
		return "Bc3SrgbBlock";
	case ze::gfx::Format::Bc7UnormBlock:
		return "Bc7UnormBlock";
	case ze::gfx::Format::Bc7SrgbBlock:
		return "Bc7SrgbBlock";
	case ze::gfx::Format::Bc5UnormBlock:
		return "Bc5UnormBlock";
	case ze::gfx::Format::Bc5SnormBlock:
		return "Bc5SnormBlock";
	case ze::gfx::Format::Bc6HUfloatBlock:
		return "Bc6HUfloatBlock";
	case ze::gfx::Format::Bc6HSfloatBlock:
		return "Bc6HSfloatBlock";
	}
}
}

namespace ze
{
ZE_FORCEINLINE std::string to_string_pretty(const ze::gfx::Format& in_format)
{
	switch (in_format)
	{
	default:
	case ze::gfx::Format::Undefined:
		return "Undefined";
	case ze::gfx::Format::D24UnormS8Uint:
		return "Depth 24-bit (unorm) Stencil 8-bit (uint)";
	case ze::gfx::Format::D32Sfloat:
		return "Depth 32-bit (float)";
	case ze::gfx::Format::D32SfloatS8Uint:
		return "Depth 32-bit (float) Stencil 8-bit (uint)";
	case ze::gfx::Format::B8G8R8A8Unorm:
		return "BGRA 8-bit (unorm)";
	case ze::gfx::Format::R8G8B8Unorm:
		return "RGB 8-bit (unorm)";
	case ze::gfx::Format::R8G8B8A8Unorm:
		return "RGBA 8-bit (unorm)";
	case ze::gfx::Format::R8G8B8A8Srgb:
		return "RGBA 8-bit (sRGB)";
	case ze::gfx::Format::R16G16B16A16Sfloat:
		return "RGBA 16-bit (float)";
	case ze::gfx::Format::R32Uint:
		return "R 32-bit (uint)";
	case ze::gfx::Format::R64Uint:
		return "R 64-bit (uint)";
	case ze::gfx::Format::R32G32Sfloat:
		return "RG 32-bit (float)";
	case ze::gfx::Format::R32G32B32Sfloat:
		return "RGB 32-bit (float)";
	case ze::gfx::Format::R32G32B32A32Sfloat:
		return "RGBA 32-bit (float)";
	case ze::gfx::Format::R32G32B32A32Uint:
		return "RGBA 32-bit (uint)";
	case ze::gfx::Format::Bc1RgbUnormBlock:
		return "BC1 RGB";
	case ze::gfx::Format::Bc1RgbaUnormBlock:
		return "BC1 RGBA";
	case ze::gfx::Format::Bc1RgbSrgbBlock:
		return "BC1 RGB (sRGB)";
	case ze::gfx::Format::Bc1RgbaSrgbBlock:
		return "BC1 RGBA (sRGB)";
	case ze::gfx::Format::Bc3UnormBlock:
		return "BC3";
	case ze::gfx::Format::Bc3SrgbBlock:
		return "BC3 (sRGB)";
	case ze::gfx::Format::Bc7UnormBlock:
		return "BC7";
	case ze::gfx::Format::Bc7SrgbBlock:
		return "BC7 (sRGB)";
	case ze::gfx::Format::Bc5UnormBlock:
		return "BC5 (unorm)";
	case ze::gfx::Format::Bc5SnormBlock:
		return "BC5 (snorm)";
	case ze::gfx::Format::Bc6HUfloatBlock:
		return "BC6H (ufloat)";
	case ze::gfx::Format::Bc6HSfloatBlock:
		return "BC6H (sfloat)";
	}
}

}