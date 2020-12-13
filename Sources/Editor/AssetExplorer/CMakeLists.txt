ze_create_module(AssetExplorer Private/Editor/AssetExplorer.cpp)

ze_add_private_modules(AssetExplorer
	Core Reflection ImGui)
ze_add_private_modules(AssetExplorer AssetDatabase AssetUtils ZEFS AssetActions Asset NotificationSystem)

target_include_directories(AssetExplorer
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)