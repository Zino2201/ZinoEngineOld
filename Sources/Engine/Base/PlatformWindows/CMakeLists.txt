ze_create_module(PlatformWindows 
    Private/PlatformWindows.cpp)

ze_add_public_modules(PlatformWindows Core Platform)
ze_add_private_modules(PlatformWindows)

target_include_directories(PlatformWindows
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)