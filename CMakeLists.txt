cmake_minimum_required(VERSION 3.13)

project(ZinoEngine)

# TODO: Make a UnrealBuildTool-like target system

# ZinoEngine only supports x64 platforms 
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "32-bits is not supported")
endif()

# Include the right .cmake file based on the targe compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	include(CMake/Clang.cmake)
else()
	message(FATAL_ERROR "Compiler not supported")
endif()

# Options
option(MONOLITHIC "Build all modules as static libraries" OFF)

# Common variables used by Modules

if(MONOLITHIC)
	set(ZE_CONFIG_NAME "$<$<CONFIG:Debug>:DebugMonolithic>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfoMonolithic>$<$<CONFIG:Release>:ReleaseMonolithic>")
else()
	set(ZE_CONFIG_NAME "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:Release>:Release>")
endif()

# Util variable for linking third-party libs
set(ZE_CONFIG_NAME_DBGREL "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Release>$<$<CONFIG:Release>:Release>")

set(ZE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ZE_BINS_DIR ${ZE_ROOT_DIR}/Binaries/${ZE_CONFIG_NAME})
set(ZE_BUILD_DIR ${CMAKE_BINARY_DIR})
set(ZE_REFL_DIR ${ZE_BUILD_DIR}/Reflection)
set(ZE_LIBS_DIR ${ZE_ROOT_DIR}/Sources/Libs)

set(ZE_ZRT_SKIP "ZRT:SKIP")

# Used for monolithic builds
add_library(AllModules INTERFACE)

# Create a new module
# This function is required to properly create a module
# It adds some compile options and preprocessor definitions required
function(ze_create_module TARGET)
	if(MONOLITHIC)
		add_library(${TARGET} STATIC ${ARGN})
		target_link_libraries(AllModules INTERFACE ${TARGET})
		target_link_options(AllModules INTERFACE "/WHOLEARCHIVE:${TARGET}.lib ")
	else()
		add_library(${TARGET} SHARED ${ARGN})
	endif()

	_ze_base_module_def(${TARGET})
endfunction()

# Create a executable module
function(ze_create_exec_module TARGET)
	add_executable(${TARGET} ${ARGN})

	_ze_base_module_def(${TARGET})
endfunction()

# Basic module definition
function(_ze_base_module_def TARGET)
	string(TOUPPER "${TARGET}" TARGET_UPPER)

	# Set bin directory
	set_target_properties(${TARGET} PROPERTIES 
		ARCHIVE_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
		LIBRARY_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
		RUNTIME_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
		VS_DEBUGGER_WORKING_DIRECTORY "${ZE_ROOT_DIR}")

	target_link_directories(${TARGET} PRIVATE "${ZE_BINS_DIR}")

	# Add _API macro
	if(MONOLITHIC)
		target_compile_definitions(${TARGET} PRIVATE "${TARGET_UPPER}_API=")
		target_compile_definitions(${TARGET} INTERFACE "${TARGET_UPPER}_API=")
		target_compile_definitions(${TARGET} PRIVATE "ZE_MONOLITHIC=1")
	else()
		target_compile_definitions(${TARGET} PRIVATE "${TARGET_UPPER}_API=ZE_DLLEXPORT")
		target_compile_definitions(${TARGET} INTERFACE "${TARGET_UPPER}_API=ZE_DLLIMPORT")
		target_compile_definitions(${TARGET} PRIVATE "ZE_MONOLITHIC=0")
	endif()

	# Config specific defs
	target_compile_definitions(${TARGET} PRIVATE ZE_CONFIGURATION_NAME="${ZE_CONFIG_NAME}")
	target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:ZE_DEBUG>$<$<CONFIG:RelWithDebInfo>:ZE_RELEASE>$<$<CONFIG:Release>:ZE_RELEASE>")

	# Disable RTTI & exceptions
	# Enable SSE4.2
	target_compile_options(${TARGET} PRIVATE 
		${ZE_COMPILE_OPT_NO_RTTI} ${ZE_COMPILE_OPT_NO_EXCEPTIONS}
		${ZE_COMPILE_OPT_SSE_4_2})

	# Generate reflection data
	set(TARGET_REFL_DIR "${ZE_REFL_DIR}/${TARGET}")

	# Parse each header to generate a ZRT file
	file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/**.h)
	
	# Clear ZRT file
	set(ZRT_FILE "${ZE_REFL_DIR}/ZRT/${TARGET}.zrt")
	file(WRITE ${ZRT_FILE} "")

	# Open each header
	foreach(HEADER_FILENAME IN LISTS HEADERS)
		file(READ ${HEADER_FILENAME} HEADER)
		
		# Don't continue if ZRT will skip this file
		string(FIND "${HEADER}" "${ZE_ZRT_SKIP}" SKIP_FOUND)
		if(NOT ${SKIP_FOUND} EQUAL -1)
			continue()
		endif()

		# Search for reflected types
		string(FIND "${HEADER}" "ZENUM" ZENUM_FOUND)
		string(FIND "${HEADER}" "ZCLASS" ZCLASS_FOUND)
		string(FIND "${HEADER}" "ZSTRUCT" ZSTRUCT_FOUND)
		
		# If a reflected type has been found
		# Add this header to the ZRT file and 
		# pre-create a .gen.cpp
		if(NOT ${ZENUM_FOUND} EQUAL -1 OR
			NOT ${ZCLASS_FOUND} EQUAL -1 OR
			NOT ${ZSTRUCT_FOUND} EQUAL -1)

			file(APPEND ${ZRT_FILE} "${HEADER_FILENAME}\n")

			get_filename_component(GENFILENAME ${HEADER_FILENAME} NAME_WLE)
			set(GENFILEPATH "${TARGET_REFL_DIR}/${GENFILENAME}.gen.cpp")
			if(NOT EXISTS ${GENFILEPATH})
				file(WRITE ${GENFILEPATH} "/** Placeholder for ZinoReflectionTool. Do not modify as this file is automaticly generated. */")
			endif()

			# Add .gen.cpp to sources
			target_sources(${TARGET} PRIVATE ${TARGET_REFL_DIR}/${GENFILENAME}.gen.cpp)
		endif()
	endforeach()

	# Add reflection headers
	target_include_directories(${TARGET} PUBLIC ${TARGET_REFL_DIR})

	# Most generators doesn't support PRE_BUILD commands
	# So we need to use a custom target with the command and add a dependency to it
	add_custom_target(${TARGET}_WORKAROUND_PREBUILD ALL
				COMMAND "${ZE_BINS_DIR}/ZinoReflectionTool.exe" -Module="${TARGET}" -SrcDir="${CMAKE_CURRENT_SOURCE_DIR}" -OutDir="${TARGET_REFL_DIR}/" ${ZRT_FILE})
	add_dependencies(${TARGET}_WORKAROUND_PREBUILD ZinoReflectionTool)
	add_dependencies(${TARGET} ${TARGET}_WORKAROUND_PREBUILD)
endfunction()

# Helper function to add modules publicly
function(ze_add_public_modules TARGET)
	target_include_directories(${TARGET} PUBLIC ${ARGN})
	target_link_libraries(${TARGET} PUBLIC ${ARGN})
endfunction()

# Helper function to add modules privately
function(ze_add_private_modules TARGET)
	target_include_directories(${TARGET} PRIVATE ${ARGN})
	target_link_libraries(${TARGET} PRIVATE ${ARGN})
	foreach(MODULE IN LISTS ARGN)
		set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS "/WHOLEARCHIVE:${MODULE}.lib ")
	endforeach()		
endfunction()

add_subdirectory(Sources)
