add_library(assetutils
    private/editor/assetutils/assetutils.cpp
    public/editor/assetutils/assetutils.h)

target_link_libraries(assetutils
    PRIVATE
        core
        asset)

target_include_directories(assetutils
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/stb_image
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)