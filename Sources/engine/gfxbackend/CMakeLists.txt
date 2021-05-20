add_library(gfxbackend
    private/GfxBackend.cpp
    public/gfx/Backend.h
    public/gfx/Resource.h)

target_link_libraries(gfxbackend PUBLIC core)

target_include_directories(gfxbackend
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)