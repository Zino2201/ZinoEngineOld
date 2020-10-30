ze_create_module(MapEditor Private/Editor/EntityList.cpp
    Private/Editor/EntityProperties.cpp Private/Editor/MapEditor.cpp)

ze_add_private_modules(MapEditor Core ShaderCore Engine Reflection ImGui)

target_include_directories(MapEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)