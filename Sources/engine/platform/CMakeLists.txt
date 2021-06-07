add_library(platform
    private/PlatformMgr.cpp
    public/PlatformMgr.h
    public/Device.h
    public/Platform.h)

target_link_libraries(platform PRIVATE core zefs tomlplusplus::tomlplusplus)

target_include_directories(platform
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)