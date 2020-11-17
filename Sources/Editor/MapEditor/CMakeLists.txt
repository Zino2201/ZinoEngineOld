find_package(SDL2 CONFIG REQUIRED)

ze_create_module(MapEditor Private/Editor/EntityList.cpp
    Private/Editor/EntityProperties.cpp Private/Editor/MapEditor.cpp
    Private/Editor/EntityProfiler.cpp)

ze_add_private_modules(MapEditor Core ShaderCore Engine Reflection ImGui PropertyEditor)

target_include_directories(MapEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)

    
target_link_libraries(MapEditor PRIVATE SDL2::SDL2)