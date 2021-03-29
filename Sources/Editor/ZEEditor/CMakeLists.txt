find_package(SDL2 CONFIG REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

ze_create_module(ZEEditor 
    Assets/ZEEditor.qrc
    Public/Editor/MainWindow.h
    Public/Editor/Windows/AssetExplorer.h
    Public/Editor/Windows/EntityProperties.h
    Public/Editor/Windows/Viewport.h
    Public/Editor/Windows/EntityList.h
    Private/Editor/Assets/Factories/AssetFactory.cpp
    Private/Editor/Assets/Actions/TextureActions.cpp
    Private/Editor/Assets/Actions/ModelActions.cpp
    Private/Editor/Assets/Factories/StbTextureFactory.cpp
    Private/Editor/Assets/Factories/TinyObjFactory.cpp
    Private/Editor/Widgets/MapTabWidget.cpp
    Private/Editor/Widgets/TextureEditorTab.cpp
    Private/Editor/Widgets/ModelEditorTab.cpp
    Private/Editor/Widgets/MapEditorTab.cpp
    Private/Editor/Widgets/AssetExplorer.cpp
    Private/Editor/Widgets/MapEditor/EntityList.cpp
    Private/Editor/Widgets/MapEditor/EntityProperties.cpp
    Private/Editor/Windows/AssetExplorer.cpp
    Private/Editor/Windows/EntityProperties.cpp
    Private/Editor/Windows/Viewport.cpp
    Private/Editor/Windows/EntityList.cpp
	Private/Editor/ZEEditor.cpp
	Private/Editor/AssetExplorer.cpp
	Private/Editor/MainWindow.cpp
	Private/Editor/Widgets/ConvarViewer.cpp)

set_target_properties(ZEEditor
    PROPERTIES
    INCLUDE_CURRENT_DIR ON 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON)

ze_add_public_modules(ZEEditor ZEUI QtAdvancedDockingSystem)
ze_add_private_modules(ZEEditor
	Core ShaderCore Engine Reflection ImGui MapEditor AssetDatabase AssetUtils PropertyEditor AssetActions NotificationSystem
    SDFFontGen)
	
target_include_directories(ZEEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(ZEEditor PUBLIC Qt5::Core Qt5::Widgets)
target_link_libraries(ZEEditor PRIVATE SDL2::SDL2)