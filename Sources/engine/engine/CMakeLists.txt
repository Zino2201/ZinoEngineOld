add_library(engine
    private/Engine/Assets/Texture.cpp
    private/Engine/Assets/Model.cpp
    private/Engine/Assets/TexturePlatformData.cpp
    private/Engine/UI/Console.cpp
    private/engine/Engine.cpp
    private/engine/EngineGame.cpp
    private/engine/InputSystem.cpp
    private/engine/TickSystem.cpp
    private/engine/Viewport.cpp
    private/engine/NativeWindow.cpp
    private/engine/ECS/ComponentManager.cpp
    private/engine/ECS/EntityManager.cpp
    private/engine/ECS/SystemManager.cpp
    private/engine/Components/SinusMovementSystem.cpp
    private/engine/World.cpp)

target_link_libraries(engine PUBLIC core reflection zefs shadercore imgui asset assetdatabase gfxbackend)

target_include_directories(engine
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)

target_link_libraries(engine PRIVATE SDL2)
if(ZE_WITH_EDITOR)
    target_link_libraries(engine PRIVATE assetdatacache)
endif()