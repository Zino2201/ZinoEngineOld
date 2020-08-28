ze_create_module(ShaderCore Private/Shader/ShaderCore.cpp)
ze_add_public_modules(ShaderCore Core)

target_include_directories(ShaderCore
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)