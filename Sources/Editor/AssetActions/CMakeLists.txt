ze_create_module(AssetActions 
    Private/Editor/Assets/Actions/AssetActions.cpp)

ze_add_private_modules(AssetActions
	Core Reflection Asset)
	
target_include_directories(AssetActions
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)