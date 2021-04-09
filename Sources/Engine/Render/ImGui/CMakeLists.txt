find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ImGui Private/ImGui/ImGuiModule.cpp Private/ImGui/ImGuiRenderer.cpp
    ${ZE_LIBS_DIR}/imgui/imgui.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_demo.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_draw.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_widgets.cpp
    ${ZE_LIBS_DIR}/imgui/imgui_tables.cpp
    ${ZE_LIBS_DIR}/imgui/backends/imgui_impl_sdl.cpp)
ze_add_public_modules(ImGui Core ShaderCore ShaderCompiler Gfx)
ze_add_private_modules(ImGui ZEFS)

target_include_directories(ImGui
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/imgui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(ImGui PRIVATE SDL2::SDL2)
target_compile_definitions(ImGui PUBLIC -DIMGUI_DISABLE_INCLUDE_IMCONFIG_H -DIMGUI_USER_CONFIG="ImGuiConfigZE.h")