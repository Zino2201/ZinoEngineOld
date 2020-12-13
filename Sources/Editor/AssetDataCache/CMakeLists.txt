ze_create_module(AssetDataCache 
    Public/AssetDataCache/AssetDataCache.h
    Private/AssetDataCache/AssetDataCache.cpp)

ze_add_private_modules(AssetDataCache Core ZEFS)
	
target_include_directories(AssetDataCache
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)