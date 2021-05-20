#pragma once

#include "logger/Logger.h"

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
#if ZE_FEATURE(DEVELOPMENT)
#define ZE_CHECK(condition) if(!(condition)) { ze::logger::error("Check failed: {} (File: {}, Line: {})", #condition, __FILE__, __LINE__); ZE_DEBUGBREAK(); }
#define ZE_CHECKF(condition, msg, ...) if(!(condition)) { ze::logger::error("{} (File: {}, Line: {})", fmt::format(msg, __VA_ARGS__), __FILE__, __LINE__); ZE_DEBUGBREAK(); }
#else
#define ZE_CHECK(condition)
#define ZE_CHECKF(condition, msg, ...)
#endif