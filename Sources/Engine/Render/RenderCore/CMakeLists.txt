find_package(SDL2 CONFIG REQUIRED)

ze_create_module(RenderCore Private/Render/Shader/BasicShader.cpp Private/Render/Shader/Shader.cpp)

ze_add_public_modules(RenderCore Core RenderSystem ShaderCore ShaderCompiler)

target_include_directories(RenderCore
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(RenderCore PRIVATE SDL2::SDL2)