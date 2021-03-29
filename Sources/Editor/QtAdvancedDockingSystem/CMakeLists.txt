find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(QT_ADVANCED_DOCKING_SYSTEM_SRCS ${ZE_LIBS_DIR}/Qt-Advanced-Docking-System/src)

ze_create_module(QtAdvancedDockingSystem
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/ads.qrc
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/ads_globals.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/ads_globals.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaTabBar.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaTabBar.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaTitleBar.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaTitleBar.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaTitleBar_p.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaWidget.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockAreaWidget.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockComponentsFactory.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockComponentsFactory.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockContainerWidget.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockContainerWidget.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockFocusController.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockFocusController.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockingStateReader.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockingStateReader.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockManager.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockManager.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockOverlay.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockOverlay.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockSplitter.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockSplitter.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockWidget.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockWidget.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockWidgetTab.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/DockWidgetTab.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/ElidingLabel.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/ElidingLabel.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/FloatingDockContainer.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/FloatingDockContainer.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/FloatingDragPreview.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/FloatingDragPreview.h
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/IconProvider.cpp
    ${QT_ADVANCED_DOCKING_SYSTEM_SRCS}/IconProvider.h)

set_target_properties(QtAdvancedDockingSystem
    PROPERTIES
    INCLUDE_CURRENT_DIR ON 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON)

target_compile_definitions(QtAdvancedDockingSystem PRIVATE ADS_SHARED_EXPORT)
	
target_include_directories(QtAdvancedDockingSystem
    PUBLIC 
        ${QT_ADVANCED_DOCKING_SYSTEM_SRCS})

target_link_libraries(QtAdvancedDockingSystem PUBLIC Qt5::Core Qt5::Widgets)