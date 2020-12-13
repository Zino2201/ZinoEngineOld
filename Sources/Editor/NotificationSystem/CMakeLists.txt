ze_create_module(NotificationSystem
        Private/NotificationSystem.cpp)

ze_add_private_modules(NotificationSystem Core)

target_include_directories(NotificationSystem
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)