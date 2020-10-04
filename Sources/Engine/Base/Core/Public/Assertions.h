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
#define must(condition) if(!(condition)) { ZE::Logger::Fatal("Assertion failed: {}", #condition); }
#ifdef ZE_DEBUG
#define verify(condition) if(!(condition)) { ZE::Logger::Error("Verify assertion failed: {}", #condition); ZE_DEBUGBREAK(); }
#else
#define verify(condition)
#endif