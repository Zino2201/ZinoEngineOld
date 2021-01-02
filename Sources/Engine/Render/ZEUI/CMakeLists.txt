find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ZEUI 
	Public/ZEUI/Container.h
	Public/ZEUI/Item.h
	Public/ZEUI/Widget.h
	Public/ZEUI/Window.h
	Public/ZEUI/CompositeWidget.h
	Public/ZEUI/Primitives/Canvas.h
	Private/ZEUI/Primitives/Text.cpp
	Private/ZEUI/Primitives/Canvas.cpp
	Private/ZEUI/Container.cpp
	Private/ZEUI/CompositeWidget.cpp
	Private/ZEUI/Window.cpp
	Private/ZEUI/Widget.cpp)
ze_add_public_modules(ZEUI Core GfxCore)
ZE_ADD_PRIVATE_MODULES(ZEUI ImGui)

target_include_directories(ZEUI
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(ZEUI PRIVATE SDL2::SDL2)