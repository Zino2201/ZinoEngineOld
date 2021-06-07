add_executable(main
    private/Main.cpp)

set_target_properties(main PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${ZE_BINS_DIR}"
    VS_DEBUGGER_WORKING_DIRECTORY "${ZE_ROOT_DIR}")

if(ZE_WITH_EDITOR)
    set_target_properties(main PROPERTIES OUTPUT_NAME "ZEEditor")
else()
    set_target_properties(main PROPERTIES OUTPUT_NAME "ZEGame")
endif()

target_link_libraries(main
    PRIVATE
        core zefs engine gfx vulkangfx SDL2)

if(ZE_WITH_EDITOR)
    target_link_libraries(main PRIVATE editor vulkanshadercompiler)
    target_compile_definitions(main PRIVATE "ZE_WITH_EDITOR=1")
else()
    target_compile_definitions(main PRIVATE "ZE_WITH_EDITOR=0")
endif()

if(ZE_MONOLITHIC)
    target_link_libraries(main PRIVATE AllModules)
endif()

# Config specific defs
target_compile_definitions(main PRIVATE ZE_CONFIGURATION_NAME="${ZE_CONFIG_NAME}")
target_compile_definitions(main PRIVATE "$<$<CONFIG:Debug>:ZE_DEBUG>$<$<CONFIG:RelWithDebInfo>:ZE_RELWITHDEBINFO>$<$<CONFIG:Release>:ZE_RELEASE>")