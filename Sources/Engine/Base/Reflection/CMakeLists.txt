ze_create_module(Reflection Private/Reflection/Class.cpp
	Private/Reflection/Reflection.cpp Private/Reflection/Struct.cpp
	Private/Reflection/Type.cpp)

ze_add_public_modules(Reflection Core)

target_include_directories(Reflection
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)