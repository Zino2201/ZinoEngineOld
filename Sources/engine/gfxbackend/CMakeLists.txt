add_library(gfxbackend
    private/BackendManager.cpp
    private/GfxBackend.cpp
    public/gfx/Backend.h
    public/gfx/Resource.h
    public/gfx/BackendManager.h
    public/gfx/ShaderFormat.h
    public/gfx/BackendInfo.h
    public/gfx/BackendModule.h)

target_link_libraries(gfxbackend PUBLIC core PRIVATE zefs tomlplusplus::tomlplusplus)

target_include_directories(gfxbackend
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)