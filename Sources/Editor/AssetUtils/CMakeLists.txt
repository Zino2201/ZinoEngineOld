ze_create_module(AssetUtils Private/Editor/AssetUtils/AssetUtils.cpp)

ze_add_private_modules(AssetUtils Core Asset AssetDatabase)

target_include_directories(AssetUtils
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/stb_image
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)