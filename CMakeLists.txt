cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0091 NEW)

# ZinoEngine only supports x64 platforms
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "32-bits is not supported")
endif()

# Options
option(ZE_MONOLITHIC "Build all modules as static libraries" OFF)
option(ZE_WITH_EDITOR "Build with the editor" OFF)

# Common variables used by Modules

if(ZE_MONOLITHIC)
	if(ZE_WITH_EDITOR)
		set(ZE_CONFIG_NAME "$<$<CONFIG:Debug>:Debug-Monolithic-Editor>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo-Monolithic-Editor>$<$<CONFIG:Release>:Release-Monolithic-Editor>")
	else()
		set(ZE_CONFIG_NAME "$<$<CONFIG:Debug>:Debug-Monolithic>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo-Monolithic>$<$<CONFIG:Release>:Release-Monolithic>")
	endif()
else()
	if(ZE_WITH_EDITOR)
		set(ZE_CONFIG_NAME "$<$<CONFIG:Debug>:Debug-Editor>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo-Editor>$<$<CONFIG:Release>:Release-Editor>")
	else()
		set(ZE_CONFIG_NAME "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:Release>:Release>")
	endif()
endif()

# Util variable for linking third-party libs
set(ZE_CONFIG_NAME_DBGREL "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Release>$<$<CONFIG:Release>:Release>")

set(ZE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ZE_BINS_DIR ${ZE_ROOT_DIR}/Binaries/${ZE_CONFIG_NAME})
set(ZE_BINS_PREFIX "ZE-")
set(ZE_BUILD_DIR ${CMAKE_BINARY_DIR})
set(ZE_REFL_DIR ${ZE_BUILD_DIR}/reflection)
set(ZE_LIBS_DIR ${ZE_ROOT_DIR}/Sources/thirdparty)

set(ZE_ZRT_SKIP "#zert skip")

# Put all libs in a seperate project in order to not pollute the main ZinoEngine project
project(ZinoEngine3rdParty)

# Include the right .cmake file based on the target compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	include(CMake/Clang.cmake)
else()
	message(FATAL_ERROR "Compiler not supported")
endif()

add_subdirectory(Sources/thirdparty)

project(ZinoEngine)

# Used for monolithic builds
add_library(AllModules INTERFACE)

add_subdirectory(Sources)