ze_create_module(JsonSerialization 
    Public/Serialization/Json.h
    Private/Serialization/Json.cpp)

ze_add_public_modules(JsonSerialization Core)

target_include_directories(JsonSerialization
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/rapidjson/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)