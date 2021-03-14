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
	
	/** BC3/DXT5 */
	Bc3UnormBlock,
	
	/** BC5 */
	Bc5UnormBlock,
	Bc5SnormBlock,

	/** BC6H */
	Bc6HUfloatBlock,
	Bc6HSfloatBlock,

	/** BC7 */
	Bc7UnormBlock,
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
	case ze::gfx::Format::Bc3UnormBlock:
		return "Bc3UnormBlock";
	case ze::gfx::Format::Bc7UnormBlock:
		return "Bc7UnormBlock";
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