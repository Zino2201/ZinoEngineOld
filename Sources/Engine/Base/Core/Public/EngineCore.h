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