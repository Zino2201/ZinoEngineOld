#pragma once

/** Detect unsupported compilers/platforms */
#ifndef _MSC_VER
#error "This compiler is not yet supported"
#endif

#ifndef _WIN32
#error "ZinoEngine doesn't support this platform yet"
#endif

/**
 * Minimal include for classes
 */
#include <cstdint>

/** Maths */
#include "Maths/MathCore.h"
#include "Maths/Transform.h"
#include "Maths/Vector.h"

/** Memory */
#include "Memory/SmartPointers.h"

#include "Logger/Logger.h"

/** Macros */
#define SDL_MAIN_HANDLED
#define UNUSED_VARIABLE(Var) (void)(Var)

/** Platform specific thing */
#ifdef _WIN32
#define NOMINMAX
#endif

/** Compiler specific things */
#ifdef _MSC_FULL_VER
#define FORCEINLINE __forceinline
#define RESTRICT __restrict

/** Fix for __declspec(dllexport) templates on MSVC */
#pragma warning(disable: 4251)

/** Dllexport/dllimport */
#ifdef ZE_MONOLITHIC
#define DLLEXPORT
#define DLLIMPORT
#else
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#endif /** ZE_MONOLITHIC */
#endif /** _MSC_VER */

/** Assertions */
#define must(condition) if(!(condition)) { ZE::Logger::Fatal("Assertion failed: {}", #condition); }
#ifdef _DEBUG
#define verify(condition) if(!(condition)) __debugbreak()
#else
#define verify(condition)
#endif

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

/** Flags */
#include <type_traits>
#include "Flags/Flags.h"

namespace ZE
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

#define HAS_FLAG(Enum, Other) (Enum & Other) == Other
#define HASN_FLAG(Enum, Other) !(HAS_FLAG(Enum, Other))

} /* namespace ZE */