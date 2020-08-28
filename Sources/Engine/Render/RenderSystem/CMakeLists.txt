ze_create_module(RenderSystem Private/Render/RenderSystem/RenderSystemResources.cpp)

ze_add_public_modules(RenderSystem Core ShaderCore)

target_include_directories(RenderSystem
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)