add_library(effect
    private/gfx/effect/Effect.cpp
    private/gfx/effect/EffectDatabase.cpp
    private/gfx/effect/EffectSystem.cpp
    private/gfx/effect/EffectCompiler.cpp
    public/gfx/effect/Effect.h
    public/gfx/effect/EffectDatabase.h
    public/gfx/effect/EffectCompiler.h)

target_link_libraries(effect
    PUBLIC
        core
        shadercore
        gfx
        platform
    PRIVATE
        zefs)

if(ZE_WITH_EDITOR)
    target_link_libraries(effect PUBLIC shadercompiler assetdatacache)
endif()

target_include_directories(effect
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)