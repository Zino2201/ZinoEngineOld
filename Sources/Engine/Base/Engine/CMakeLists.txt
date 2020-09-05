find_package(SDL2 CONFIG REQUIRED)

ze_create_module(Engine Private/Engine/Assets/AssetManager.cpp Private/Engine/Assets/StaticMesh.cpp
    Private/Engine/Components/EventSystem.cpp Private/Engine/Components/RenderableComponent.cpp
        Private/Engine/Components/StaticMeshComponent.cpp Private/Engine/Components/TransformComponent.cpp
    Private/Engine/UI/Console.cpp
    Private/Engine/ECS.cpp Private/Engine/Engine.cpp Private/Engine/EngineGame.cpp Private/Engine/InputSystem.cpp
        Private/Engine/TickSystem.cpp Private/Engine/Viewport.cpp Private/Engine/World.cpp)

ze_add_public_modules(Engine Core ZEFS RenderCore ShaderCore RenderSystem ShaderCompiler Reflection ImGui Renderer)

target_include_directories(Engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(Engine PRIVATE SDL2::SDL2)