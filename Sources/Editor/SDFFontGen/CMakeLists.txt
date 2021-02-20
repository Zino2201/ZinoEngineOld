find_package(freetype CONFIG REQUIRED)

ze_create_module(SDFFontGen 
    Public/SDFFontGen.h
    Private/SDFFontGen.cpp)

ze_add_private_modules(SDFFontGen Core GfxCore)

target_include_directories(SDFFontGen
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(SDFFontGen PRIVATE freetype)