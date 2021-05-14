ze_create_module(Platform 
    Public/Device.h
    Public/Platform.h
    Public/PlatformMgr.h
    Private/PlatformMgr.cpp)

ze_add_public_modules(Platform Core)
ze_add_private_modules(Platform)

target_include_directories(Platform
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)