add_library(imgui
    private/imgui/ImGuiModule.cpp
    private/imgui/ImGuiRenderer.cpp
    ${ZE_LIBS_DIR}/imgui/imgui.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_demo.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_draw.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_widgets.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_tables.cpp
    ${ZE_LIBS_DIR}/imgui/backends/imgui_impl_sdl.cpp
    ${ZE_LIBS_DIR}/imgui/misc/cpp/imgui_stdlib.cpp)

target_link_libraries(imgui
    PUBLIC
        core
        shadercore
        effect
        gfx)

target_include_directories(imgui
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/imgui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)

target_link_libraries(imgui PRIVATE SDL2)
target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_INCLUDE_IMCONFIG_H -DIMGUI_USER_CONFIG="ImGuiConfigZE.h")