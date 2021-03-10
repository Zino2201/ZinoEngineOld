ze_create_module(Gfx
    Public/Gfx/Gfx.h
    Public/Gfx/GpuVector.h
    Private/Gfx/Gfx.cpp
    Private/Gfx/GpuVector.cpp
    Private/Gfx/CommandList.cpp
    Private/Gfx/ThreadedCommandPool.cpp
    Private/Gfx/Device.cpp)

ze_add_public_modules(Gfx Core GfxBackend)

target_include_directories(Gfx
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)