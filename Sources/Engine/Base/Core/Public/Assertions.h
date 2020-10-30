#pragma once

#include "Logger/Logger.h"

/** Portable __debugbreak */
#if ZE_COMPILER(MSVC)
#define ZE_DEBUGBREAK() __debugbreak();
#elif ZE_COMPILER(GCC) || ZE_COMPILER(CLANG)
#define ZE_DEBUGBREAK() __asm volatile ("int $0x3");
#else
#define ZE_DEBUGBREAK()
#endif /** ZE_COMPILER(MSVC) */

/** Assertions */
#define ZE_ASSERT(condition) if(!(condition)) { ze::logger::fatal("Assertion failed: {}", #condition); }
#define ZE_ASSERTF(condition, msg, ...) if(!(condition)) { ze::logger::fatal("{} ({})", fmt::format(msg, __VA_ARGS__), #condition); }
#ifdef ZE_DEBUG
#define ZE_CHECK(condition) if(!(condition)) { ze::logger::error("Check failed: {}", #condition); ZE_DEBUGBREAK(); }
#define ZE_CHECKF(condition, msg, ...) if(!(condition)) { ze::logger::error("{} (Line {})", fmt::format(msg, __VA_ARGS__), #condition); ZE_DEBUGBREAK(); }
#else
#define ZE_CHECK(condition)
#define ZE_CHECKF(condition, msg, ...)
#endif