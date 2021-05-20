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

/** Memory */
#include "memory/SmartPointers.h"

/** Macros */
#define SDL_MAIN_HANDLED
#define ZE_UNUSED_VARIABLE(Var) (void)(Var)

/** Platform specific thing */
#if ZE_PLATFORM(WINDOWS)
#define NOMINMAX
#endif

#include "logger/Logger.h"
#include "Assertions.h"

#define ZE_CONCAT_IMPL(x, y) x ## y
#define ZE_CONCAT(x, y) ZE_CONCAT_IMPL(x, y)

/** Flags */
#include <type_traits>
#include "flags/Flags.h"

namespace std
{

ZE_FORCEINLINE std::string to_string(bool value)
{
	return value ? "true" : "false";
}


}