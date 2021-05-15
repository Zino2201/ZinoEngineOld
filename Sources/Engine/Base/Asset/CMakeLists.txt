ze_create_module(Asset 
    Public/Assets/AssetArchive.h
    Public/Assets/Asset.h
    Public/Assets/AssetManager.h
    Public/Assets/AssetPtr.h
    Public/Assets/AssetMetadata.h
    Private/Assets/Asset.cpp
    Private/Assets/AssetCooker.cpp
    Private/Assets/AssetManager.cpp)

ze_add_public_modules(Asset Core Reflection ZEFS JsonSerialization Platform)
ze_add_private_modules(Asset)

target_include_directories(Asset
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/stduuid/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)