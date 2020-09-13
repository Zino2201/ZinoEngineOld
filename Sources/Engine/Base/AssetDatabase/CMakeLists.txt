ze_create_module(AssetDatabase Public/AssetDatabase/AssetDatabase.h Private/AssetDatabase/AssetDatabase.cpp
    Private/AssetDatabase/PathTree.cpp)

ze_add_public_modules(AssetDatabase Core)
ze_add_private_modules(AssetDatabase Asset ZEFS)

target_include_directories(AssetDatabase
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)