ze_create_module(PropertyEditor 
    Private/Editor/ArithmeticPropertyEditors.cpp
    Private/Editor/PropertyEditor.cpp
    Private/Editor/VectorPropertyEditor.cpp)

ze_add_private_modules(PropertyEditor Core Reflection ImGui)

target_include_directories(PropertyEditor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)