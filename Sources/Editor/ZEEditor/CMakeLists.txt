find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ZEEditor 
    Private/Editor/Assets/Factories/AssetFactory.cpp
    Private/Editor/Assets/Factories/StbTextureFactory.cpp
    Private/Editor/Widgets/MapTabWidget.cpp
	Private/Editor/ZEEditor.cpp)

ze_add_private_modules(ZEEditor
	Core RenderCore ShaderCore RenderSystem Engine Reflection ImGui Renderer MapEditor AssetExplorer AssetDatabase
    AssetUtils)
	
target_include_directories(ZEEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(ZEEditor PRIVATE SDL2::SDL2)