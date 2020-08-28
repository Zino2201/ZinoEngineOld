find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ImGui Private/ImGui/ImGuiModule.cpp Private/ImGui/ImGuiRender.cpp)
ze_add_public_modules(ImGui Core RenderCore ShaderCore ShaderCompiler RenderSystem)

target_include_directories(ImGui
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/imgui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_sources(ImGui 
    PRIVATE 
        ${ZE_LIBS_DIR}/imgui/imgui.cpp
        ${ZE_LIBS_DIR}/imgui/imgui_demo.cpp
        ${ZE_LIBS_DIR}/imgui/imgui_draw.cpp
        ${ZE_LIBS_DIR}/imgui/imgui_widgets.cpp
        ${ZE_LIBS_DIR}/imgui/examples/imgui_impl_sdl.cpp)

target_link_libraries(ImGui PRIVATE SDL2::SDL2)