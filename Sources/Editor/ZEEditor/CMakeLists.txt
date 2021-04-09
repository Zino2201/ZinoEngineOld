find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ZEEditor 
    Public/Editor/ZEEditor.h
    Public/Editor/IconManager.h
    Public/Editor/PropertyEditor.h
    Public/Editor/PropertiesEditor.h
    Public/Editor/Windows/MapEditor.h
    Public/Editor/Windows/Window.h
    Public/Editor/Windows/AssetExplorer.h
    Public/Editor/Windows/Viewport.h
    Public/Editor/Windows/EntityList.h
    Public/Editor/Windows/EntityProperties.h
    Public/Editor/Windows/Tools.h
    Public/Editor/Assets/AssetActions.h
    Public/Editor/Assets/AssetFactory.h
    Public/Editor/Assets/TextureActions.h
    Public/Editor/Windows/Assets/Texture/TextureEditor.h
    Private/Editor/Assets/Factories/AssetFactory.cpp
    Private/Editor/Assets/Actions/TextureActions.cpp
    Private/Editor/Assets/Actions/AssetActions.cpp
    Private/Editor/Assets/Actions/ModelActions.cpp
    Private/Editor/Assets/Factories/StbTextureFactory.cpp
    Private/Editor/Assets/Factories/TinyObjFactory.cpp
    Private/Editor/PropertyEditors/PrimitivesPropertyEditors.h
    Private/Editor/PropertyEditors/PrimitivesPropertyEditors.cpp
    Private/Editor/Windows/MapEditor.cpp
    Private/Editor/Windows/Window.cpp
    Private/Editor/Windows/AssetExplorer.cpp
    Private/Editor/Windows/Viewport.cpp
    Private/Editor/Windows/EntityList.cpp
    Private/Editor/Windows/EntityProperties.cpp
    Private/Editor/Windows/Tools.cpp
    Private/Editor/Windows/Console.cpp
    Private/Editor/Windows/Assets/Texture/TextureEditor.cpp
    Private/Editor/PropertiesEditor.cpp
    Private/Editor/IconManager.cpp
    Private/Editor/PropertyEditor.cpp
	Private/Editor/ZEEditor.cpp)

ze_add_public_modules(ZEEditor ZEUI)
ze_add_private_modules(ZEEditor
	Core ShaderCore Engine Reflection ImGui AssetDatabase AssetUtils NotificationSystem SDFFontGen)
	
target_include_directories(ZEEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/tinyobjloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

target_link_libraries(ZEEditor)
target_link_libraries(ZEEditor PRIVATE SDL2::SDL2)