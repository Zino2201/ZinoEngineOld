#find_package(Boost 1.76.0 REQUIRED COMPONENTS system)

add_library(shadercompiler
    private/shader/ShaderCompiler.cpp
    private/shader/ShaderCompilerModule.cpp
    public/shader/ShaderCompiler.h
    public/shader/ShaderCompilerModule.h)
target_link_libraries(shadercompiler PUBLIC gfx PRIVATE core shadercore)

target_include_directories(shadercompiler
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)

    