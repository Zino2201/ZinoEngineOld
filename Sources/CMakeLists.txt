add_subdirectory(thirdparty)

function(ze_add_library TARGET)
    if(ZE_MONOLITHIC)
        _add_library(${TARGET} STATIC ${ARGN})
    else()
        _add_library(${TARGET} SHARED ${ARGN})
    endif()
    string(TOUPPER "${TARGET}" TARGET_UPPER)

    # Set bin directory
    set_target_properties(${TARGET} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
        VS_DEBUGGER_WORKING_DIRECTORY "${ZE_ROOT_DIR}"
        PREFIX "${ZE_BINS_PREFIX}")

    target_link_directories(${TARGET} PRIVATE "${ZE_BINS_DIR}")

    # Add _API macro
    if(ZE_MONOLITHIC)
        target_compile_definitions(${TARGET} PRIVATE "${TARGET_UPPER}_API=")
        target_compile_definitions(${TARGET} INTERFACE "${TARGET_UPPER}_API=")
        target_compile_definitions(${TARGET} PRIVATE "ZE_MONOLITHIC=1")
    else()
        target_compile_definitions(${TARGET} PRIVATE "${TARGET_UPPER}_API=")
        target_compile_definitions(${TARGET} INTERFACE "${TARGET_UPPER}_API=")
        target_compile_definitions(${TARGET} PRIVATE "ZE_MONOLITHIC=0")
    endif()

    if(ZE_WITH_EDITOR)
        target_compile_definitions(${TARGET} PRIVATE "ZE_WITH_EDITOR=1")
    else()
        target_compile_definitions(${TARGET} PRIVATE "ZE_WITH_EDITOR=0")
    endif()

    # Config specific defs
    target_compile_definitions(${TARGET} PRIVATE ZE_CONFIGURATION_NAME="${ZE_CONFIG_NAME}")
    target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:ZE_DEBUG>$<$<CONFIG:RelWithDebInfo>:ZE_RELWITHDEBINFO>$<$<CONFIG:Release>:ZE_RELEASE>")

    # Disable RTTI & exceptions
    # Enable SSE4.2
    target_compile_options(${TARGET} PRIVATE
        ${ZE_COMPILE_OPT_NO_RTTI} ${ZE_COMPILE_OPT_NO_EXCEPTIONS}
        ${ZE_COMPILE_OPT_SSE_4_2} ${ZE_COMPILE_OPT_SANITIZERS})

    # Generate reflection data
    set(TARGET_REFL_DIR "${ZE_REFL_DIR}/${TARGET}")
    set(HAS_REFL_DATA FALSE)

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
                file(WRITE ${GENFILEPATH} "/** Placeholder for ZERT. Do not modify as this file is automaticly generated. */")
            endif()

            # Add .gen.cpp to sources
            target_sources(${TARGET} PRIVATE ${TARGET_REFL_DIR}/${GENFILENAME}.gen.cpp)
            SET(HAS_REFL_DATA TRUE)
        endif()
    endforeach()

    if(HAS_REFL_DATA)
        # Add reflection headers
        target_include_directories(${TARGET} PUBLIC ${TARGET_REFL_DIR})

        # Most generators doesn't support PRE_BUILD commands
        # So we need to use a custom target with the command and add a dependency to it
        add_custom_target(${TARGET}_REFLECTION_DATA ALL
            COMMAND "${ZE_BINS_DIR}/zert.exe" -Module="${TARGET}" -SrcDir="${CMAKE_CURRENT_SOURCE_DIR}" -OutDir="${TARGET_REFL_DIR}/" ${ZRT_FILE})
        add_dependencies(${TARGET}_REFLECTION_DATA zert)
        add_dependencies(${TARGET} ${TARGET}_REFLECTION_DATA)
    endif()
endfunction()

# From vcpkg
macro(z_vcpkg_function_arguments OUT_VAR)
    if("${ARGC}" EQUAL 1)
        set(z_vcpkg_function_arguments_FIRST_ARG 0)
    elseif("${ARGC}" EQUAL 2)
        set(z_vcpkg_function_arguments_FIRST_ARG "${ARGV1}")
    else()
        # vcpkg bug
        message(FATAL_ERROR "z_vcpkg_function_arguments: invalid arguments (${ARGV})")
    endif()

    set("${OUT_VAR}")

    # this allows us to get the value of the enclosing function's ARGC
    set(z_vcpkg_function_arguments_ARGC_NAME "ARGC")
    set(z_vcpkg_function_arguments_ARGC "${${z_vcpkg_function_arguments_ARGC_NAME}}")

    math(EXPR z_vcpkg_function_arguments_LAST_ARG "${z_vcpkg_function_arguments_ARGC} - 1")
    # GREATER_EQUAL added in CMake 3.7
    if(NOT z_vcpkg_function_arguments_LAST_ARG LESS z_vcpkg_function_arguments_FIRST_ARG)
        foreach(z_vcpkg_function_arguments_N RANGE "${z_vcpkg_function_arguments_FIRST_ARG}" "${z_vcpkg_function_arguments_LAST_ARG}")
            string(REPLACE ";" "\\;" z_vcpkg_function_arguments_ESCAPED_ARG "${ARGV${z_vcpkg_function_arguments_N}}")
            list(APPEND "${OUT_VAR}" "${z_vcpkg_function_arguments_ESCAPED_ARG}")
        endforeach()
    endif()
endmacro()

# Setup basic module data
function(add_library)
    z_vcpkg_function_arguments(ARGS)

    list(FIND ARGS "IMPORTED" IMPORTED_IDX)

    if(NOT IMPORTED_IDX EQUAL -1)
        _add_library(${ARGS})
    else()
        ze_add_library(${ARGS})
    endif()
endfunction()

add_subdirectory(editor)
add_subdirectory(engine)
add_subdirectory(tools)