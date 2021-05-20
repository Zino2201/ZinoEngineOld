add_library(zefs
    private/zefs/File.cpp
    private/zefs/StdFileSystem.cpp
    private/zefs/Utils.cpp
    private/zefs/ZEFS.cpp
    private/zefs/Paths.cpp
    private/zefs/sinks/FileSink.cpp)

target_link_libraries(zefs PUBLIC core)

target_include_directories(zefs
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)