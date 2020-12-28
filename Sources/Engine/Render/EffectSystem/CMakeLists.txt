ze_create_module(EffectSystem
    Public/Gfx/Effect/Effect.h
    Private/Gfx/Effect/Effect.cpp)
ze_add_public_modules(EffectSystem Core ShaderCore ShaderCompiler GfxCore AssetDataCache)

target_include_directories(EffectSystem
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)