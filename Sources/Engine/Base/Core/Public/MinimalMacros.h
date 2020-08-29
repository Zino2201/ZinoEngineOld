#pragma once


/**
 * Minmal macros
 * https://www.fluentcpp.com/2019/05/28/better-macros-better-flags/
 */

namespace ZE
{

/** Platforms */
#define ZE_PLATFORM_PRIVATE_DEFINITION_WIN32() defined(_WIN32)
#define ZE_PLATFORM_PRIVATE_DEFINITION_WIN64() defined(_WIN64)
#define ZE_PLATFORM_PRIVATE_DEFINITION_WINDOWS() ZE_PLATFORM_PRIVATE_DEFINITION_WIN32() || ZE_PLATFORM_PRIVATE_DEFINITION_WIN64()
#define ZE_PLATFORM_PRIVATE_DEFINITION_ANDROID() defined(__ANDROID__)
#define ZE_PLATFORM_PRIVATE_DEFINITION_LINUX() defined(__linux__) && !ZE_PLATFORM_PRIVATE_DEFINITION_ANDROID()
#define ZE_PLATFORM_PRIVATE_DEFINITION_OSX() defined(__APPLE__) || defined(__MACH__)
#define ZE_PLATFORM_PRIVATE_DEFINITION_FREEBSD() defined(__FREEBSD__)

/** Return 1 if compiling for this platform */
#define ZE_PLATFORM(X) ZE_PLATFORM_PRIVATE_DEFINITION_##X()

/** Compilers */
#define ZE_COMPILER_PRIVATE_DEFINITION_MSVC() defined(_MSVC_VER)
#define ZE_COMPILER_PRIVATE_DEFINITION_GCC() defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#define ZE_COMPILER_PRIVATE_DEFINITION_CLANG() defined(__clang__)
#define ZE_COMPILER_PRIVATE_DEFINITION_CLANG_CL() ZE_COMPILER_PRIVATE_DEFINITION_CLANG() && ZE_PLATFORM(WINDOWS)

/** Return 1 if compiling with the specified compiler */
#define ZE_COMPILER(X) ZE_COMPILER_PRIVATE_DEFINITION_##X()

/** Features macros */

/** Enable development-only code */
#define ZE_FEATURE_PRIVATE_DEFINITION_DEVELOPMENT() ZE_DEBUG || ZE_RELWITHDEBINFO

/** Return 1 if feature is enabled */
#define ZE_FEATURE(X) ZE_FEATURE_PRIVATE_DEFINITION_##X()

/** Dll symbol export/import */
#if ZE_COMPILER(MSVC) || ZE_COMPILER(CLANG_CL)
#define ZE_DLLEXPORT __declspec(dllexport)
#define ZE_DLLIMPORT __declspec(dllimport)
#else
#define ZE_DLLEXPORT
#define ZE_DLLIMPORT
#endif /** ZE_COMPILER(MSVC) || ZE_COMPILER(CLANG_CL) */

/** Compiler specific macros */
#if ZE_COMPILER(MSVC)
#define ZE_FORCEINLINE __forceinline
#define ZE_RESTRICT __restrict
#elif ZE_COMPILER(GCC) || ZE_COMPILER(CLANG)
#define ZE_FORCEINLINE __attribute__((always_inline)) inline
#define ZE_RESTRICT __restrict
#endif /** ZE_COMPILER(MSVC) */

}