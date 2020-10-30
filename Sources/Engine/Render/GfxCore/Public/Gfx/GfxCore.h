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
	R8G8B8A8Unorm,

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