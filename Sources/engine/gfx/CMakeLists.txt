add_library(gfx
    private/gfx/Gfx.cpp
    private/gfx/GpuVector.cpp
    private/gfx/CommandList.cpp
    private/gfx/ThreadedCommandPool.cpp
    private/gfx/Device.cpp
    public/gfx/Gfx.h
    public/gfx/GpuVector.h
    public/gfx/UniformBuffer.h)

target_link_libraries(gfx PUBLIC shadercore gfxbackend)

target_include_directories(gfx
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)