find_package(SDL2 CONFIG REQUIRED)

ze_create_module(AssetCache 
    Private/AssetCache/AssetCache.cpp)

ze_add_private_modules(AssetCache Core ZEFS)
	
target_include_directories(AssetCache
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)