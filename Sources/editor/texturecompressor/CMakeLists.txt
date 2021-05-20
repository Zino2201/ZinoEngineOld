add_library(texturecompressor
    private/TextureCompressor.cpp
    public/TextureCompressor.h)

target_link_libraries(texturecompressor PUBLIC core gfxbackend PRIVATE ${ZE_LIBS_DIR}/nvidia-texture-tools/project/vc2017/Release.x64/lib/nvtt.lib)

target_include_directories(texturecompressor
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
        ${ZE_LIBS_DIR}/nvidia-texture-tools/src/nvtt)

if(WIN32)
    target_link_directories(texturecompressor PUBLIC
        ${ZE_LIBS_DIR}/nvidia-texture-tools/project/vc2017/Release.x64/lib)
endif()