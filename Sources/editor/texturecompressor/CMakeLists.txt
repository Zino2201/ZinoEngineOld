add_library(texturecompressor
    private/TextureCompressor.cpp
    public/TextureCompressor.h)

target_link_libraries(texturecompressor PUBLIC core gfxbackend)

target_include_directories(texturecompressor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${ZE_LIBS_DIR}/nvidia-texture-tools/src/nvtt)

if(WIN32)
    if(ZE_MONOLITHIC)
        target_link_directories(texturecompressor PUBLIC
            ${ZE_LIBS_DIR}/nvidia-texture-tools/project/vc2017/${ZE_CONFIG_NAME_DBGREL}.x64/lib)
    else()
        target_link_directories(texturecompressor PRIVATE
            ${ZE_LIBS_DIR}/nvidia-texture-tools/project/vc2017/${ZE_CONFIG_NAME_DBGREL}.x64/lib)
    endif()
endif()

target_link_libraries(texturecompressor PRIVATE nvtt gfxbackend)
