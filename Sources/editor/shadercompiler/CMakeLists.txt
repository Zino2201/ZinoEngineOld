add_library(shadercompiler
    private/shader/ShaderCompiler.cpp
    public/shader/ShaderCompiler.h)
target_link_libraries(shadercompiler PRIVATE core shadercore)

target_include_directories(shadercompiler
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)