#pragma once

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

// retrocompatibility

}

namespace ZE { using EFormat = ze::gfx::Format; }