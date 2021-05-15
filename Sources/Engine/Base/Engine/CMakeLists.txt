find_package(SDL2 CONFIG REQUIRED)

ze_create_module(Engine 
    Private/Engine/Assets/Texture.cpp
    Private/Engine/Assets/Model.cpp
    Private/Engine/Assets/TexturePlatformData.cpp
    Private/Engine/UI/Console.cpp
    Private/Engine/Engine.cpp Private/Engine/EngineGame.cpp Private/Engine/InputSystem.cpp
    Private/Engine/TickSystem.cpp Private/Engine/Viewport.cpp
    Private/Engine/NativeWindow.cpp Private/Engine/ECS/ComponentManager.cpp Private/Engine/ECS/EntityManager.cpp
    Private/Engine/ECS/SystemManager.cpp Private/Engine/Components/SinusMovementSystem.cpp
    Private/Engine/World.cpp)

ze_add_public_modules(Engine Core Reflection ZEFS ShaderCore ShaderCompiler ImGui Asset AssetDatabase GfxCore)

if(ZE_WITH_EDITOR)
    ze_add_public_modules(Engine AssetDataCache TextureCompressor NotificationSystem)
endif()

target_include_directories(Engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(Engine PRIVATE SDL2::SDL2)