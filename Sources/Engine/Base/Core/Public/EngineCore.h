#pragma once

/**
 * Minimal include
 */

#include "MinimalMacros.h"

/** Detect unsupported compilers/platforms */
#if !ZE_COMPILER(CLANG)
#error "This compiler is not supported"
#endif

#if !ZE_PLATFORM(WIN64) && !ZE_PLATFORM(LINUX)
#error "Target platform is not supported"
#endif

#include <cstdint>

/** Maths */
#include "Maths/MathCore.h"
#include "Maths/Transform.h"
#include "Maths/Vector.h"

/** Memory */
#include "Memory/SmartPointers.h"

/** Macros */
#define SDL_MAIN_HANDLED
#define UNUSED_VARIABLE(Var) (void)(Var)

/** Platform specific thing */
#if ZE_PLATFORM(WINDOWS)
#define NOMINMAX
#endif

#include "Logger/Logger.h"
#include "Assertions.h"

#define ZE_CONCAT_(x, y) x ## y
#define ZE_CONCAT(x, y) ZE_CONCAT_(x, y)

/** Flags */
#include <type_traits>
#include "Flags/Flags.h"

namespace ze
{

/** Format */
enum class EFormat
{
	Undefined,
	D32Sfloat,
	D32SfloatS8Uint,
	D24UnormS8Uint,
	R32Uint, /** uint32_t */
	R8G8B8A8UNorm, /** rgba 255 */
    B8G8R8A8UNorm, /** bgra 255 */
	R32G32Sfloat, /** vec2*/
	R32G32B32Sfloat, /** vec3 */
	R32G32B32A32Sfloat,	/** vec4 */
    R32G32B32A32Uint,
	R64Uint	/** uint64 */
};

enum class ESampleCount
{	
	Sample1 = 1 << 0,
	Sample2 = 1 << 1,
	Sample4 = 1 << 2,
	Sample8 = 1 << 3, 
	Sample16 = 1 << 4,
	Sample32 = 1 << 5,
	Sample64 = 1 << 6
};

} /* namespace ze */