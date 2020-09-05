ze_create_module(ZEFS Private/ZEFS/File.cpp
    Private/ZEFS/StdFileSystem.cpp
    Private/ZEFS/Utils.cpp
    Private/ZEFS/ZEFS.cpp
    Private/ZEFS/Paths.cpp
    Private/ZEFS/Sinks/FileSink.cpp)

ze_add_public_modules(ZEFS Core)

target_include_directories(ZEFS
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)