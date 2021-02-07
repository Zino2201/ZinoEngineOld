find_package(SDL2 CONFIG REQUIRED)

ze_create_module(ZEUI 
	Public/ZEUI/Container.h
	Public/ZEUI/Item.h
	Public/ZEUI/Widget.h
	Public/ZEUI/Window.h
	Public/ZEUI/CompositeWidget.h
	Public/ZEUI/Primitives/Canvas.h
	Public/ZEUI/Primitives/Text.h
	Public/ZEUI/Primitives/DockSpace.h
	Public/ZEUI/Primitives/DockableTab.h
	Public/ZEUI/Primitives/DockTabBar.h
	Public/ZEUI/Containers/HorizontalContainer.h
	Public/ZEUI/Containers/VerticalContainer.h
	Public/ZEUI/Containers/ScrollableContainer.h
	Public/ZEUI/Render/DrawCommand.h
	Public/ZEUI/Render/DrawCommandRect.h
	Public/ZEUI/Render/Renderer.h
	Private/ZEUI/Primitives/Text.cpp
	Private/ZEUI/Primitives/Canvas.cpp
	Private/ZEUI/Primitives/DockSpace.cpp
	Private/ZEUI/Primitives/DockableTab.cpp
	Private/ZEUI/Primitives/DockTabBar.cpp
	Private/ZEUI/Containers/VerticalContainer.cpp
	Private/ZEUI/Containers/HorizontalContainer.cpp
	Private/ZEUI/Containers/ScrollableContainer.cpp
	Private/ZEUI/Render/DrawCommand.cpp
	Private/ZEUI/Render/DrawCommandRect.cpp
	Private/ZEUI/Render/Renderer.cpp
	Private/ZEUI/Container.cpp
	Private/ZEUI/CompositeWidget.cpp
	Private/ZEUI/Window.cpp
	Private/ZEUI/Widget.cpp
	Private/ZEUI/ZEUI.cpp)
ze_add_public_modules(ZEUI Core GfxCore EffectSystem)
ZE_ADD_PRIVATE_MODULES(ZEUI ImGui)

target_include_directories(ZEUI
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(ZEUI PRIVATE SDL2::SDL2)