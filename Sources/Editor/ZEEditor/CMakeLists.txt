find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ZEEditor 
    Private/Editor/Assets/Factories/AssetFactory.cpp
    Private/Editor/Assets/Actions/TextureActions.cpp
    Private/Editor/Assets/Actions/ModelActions.cpp
    Private/Editor/Assets/Factories/StbTextureFactory.cpp
    Private/Editor/Assets/Factories/TinyObjFactory.cpp
    Private/Editor/Widgets/MapTabWidget.cpp
    Private/Editor/Widgets/TextureEditorTab.cpp
    Private/Editor/Widgets/ModelEditorTab.cpp
	Private/Editor/ZEEditor.cpp
	Private/Editor/Widgets/ConvarViewer.cpp)

ze_add_private_modules(ZEEditor
	Core ShaderCore Engine Reflection ImGui MapEditor AssetExplorer AssetDatabase AssetUtils PropertyEditor AssetActions NotificationSystem)
	
target_include_directories(ZEEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(ZEEditor PRIVATE SDL2::SDL2)