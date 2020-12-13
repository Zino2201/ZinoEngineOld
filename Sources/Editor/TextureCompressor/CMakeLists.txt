ze_create_module(TextureCompressor 
    Private/TextureCompressor.cpp)

ze_add_private_modules(TextureCompressor Core GfxCore)

target_include_directories(TextureCompressor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${ZE_LIBS_DIR}/nvidia-texture-tools/src/nvtt)

if(WIN32)
    if(ZE_MONOLITHIC)
        target_link_directories(TextureCompressor PUBLIC
            ${ZE_LIBS_DIR}/nvidia-texture-tools/project/vc2017/${ZE_CONFIG_NAME_DBGREL}.x64/lib)
    else()
        target_link_directories(TextureCompressor PRIVATE
            ${ZE_LIBS_DIR}/nvidia-texture-tools/project/vc2017/${ZE_CONFIG_NAME_DBGREL}.x64/lib)
    endif()
endif()

target_link_libraries(TextureCompressor PRIVATE nvtt)