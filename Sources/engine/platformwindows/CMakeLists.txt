add_library(platformwindows
    private/PlatformWindows.cpp)

target_link_libraries(platformwindows PRIVATE core platform)

target_include_directories(platformwindows
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)