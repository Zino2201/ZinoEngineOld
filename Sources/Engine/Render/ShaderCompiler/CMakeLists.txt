ze_create_module(ShaderCompiler Private/Shader/ShaderCompiler.cpp)
ze_add_public_modules(ShaderCompiler Core ShaderCore)

target_include_directories(ShaderCompiler
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)