add_library(json
    public/serialization/Json.h
    private/serialization/Json.cpp)

target_link_libraries(json core)

target_include_directories(json
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/rapidjson/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)