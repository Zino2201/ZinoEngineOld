ze_create_module(Asset 
    Public/Assets/AssetArchive.h
    Private/Assets/AssetManager.cpp)

ze_add_public_modules(Asset Core Reflection ZEFS)
ze_add_private_modules(Asset AssetDatabase)

target_include_directories(Asset
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)