ze_create_module(GfxCore 
    Private/GfxCore.cpp
    Private/GpuVector.cpp
    Private/CommandSystem.cpp)

ze_add_public_modules(GfxCore Core Reflection)

target_include_directories(GfxCore
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)