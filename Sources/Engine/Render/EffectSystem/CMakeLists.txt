ze_create_module(EffectSystem
    Public/Gfx/Effect/Effect.h
    Public/Gfx/Effect/EffectDatabase.h
    Private/Gfx/Effect/Effect.cpp
    Private/Gfx/Effect/EffectDatabase.cpp
    Private/Gfx/Effect/EffectSystem.cpp)
ze_add_public_modules(EffectSystem Core ShaderCore ShaderCompiler GfxCore Gfx AssetDataCache)
ze_add_private_modules(EffectSystem ZEFS)

target_include_directories(EffectSystem
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)