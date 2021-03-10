ze_create_module(GfxBackend
    Public/Gfx/Backend.h
    Public/Gfx/Resource.h
    Private/GfxBackend.cpp)

ze_add_public_modules(GfxBackend Core GfxCore)

target_include_directories(GfxBackend
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)