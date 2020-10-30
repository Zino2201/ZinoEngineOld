find_package(SDL2 CONFIG REQUIRED)

ze_create_module(Engine Private/Engine/Assets/Texture.cpp Private/Engine/Assets/StaticMesh.cpp
    Private/Engine/UI/Console.cpp
    Private/Engine/Engine.cpp Private/Engine/EngineGame.cpp Private/Engine/InputSystem.cpp
    Private/Engine/TickSystem.cpp Private/Engine/Viewport.cpp Private/Engine/World.cpp
    Private/Engine/NativeWindow.cpp)

ze_add_public_modules(Engine Core ZEFS RenderCore ShaderCore RenderSystem ShaderCompiler Reflection ImGui Renderer 
    Asset AssetDatabase GfxCore)

target_include_directories(Engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(Engine PRIVATE SDL2::SDL2)