find_package(freetype CONFIG REQUIRED)

set(MSDFGEN_SRCS ${ZE_LIBS_DIR}/msdfgen)
set(MSDF_ATLAS_SRCS ${ZE_LIBS_DIR}/msdf-atlas-gen)


ze_create_module(SDFFontGen 
    Public/SDFFontGen.h
    Private/SDFFontGen.cpp
    ${MSDFGEN_SRCS}/core/Contour.cpp
    ${MSDFGEN_SRCS}/core/contour-combiners.cpp
    ${MSDFGEN_SRCS}/core/edge-coloring.cpp
    ${MSDFGEN_SRCS}/core/EdgeHolder.cpp
    ${MSDFGEN_SRCS}/core/edge-segments.cpp
    ${MSDFGEN_SRCS}/core/edge-selectors.cpp
    ${MSDFGEN_SRCS}/core/equation-solver.cpp
    ${MSDFGEN_SRCS}/core/msdf-edge-artifact-patcher.cpp
    ${MSDFGEN_SRCS}/core/msdf-error-correction.cpp
    ${MSDFGEN_SRCS}/core/msdfgen.cpp
    ${MSDFGEN_SRCS}/core/rasterization.cpp
    ${MSDFGEN_SRCS}/core/render-sdf.cpp
    ${MSDFGEN_SRCS}/core/save-bmp.cpp
    ${MSDFGEN_SRCS}/core/save-tiff.cpp
    ${MSDFGEN_SRCS}/core/Scanline.cpp
    ${MSDFGEN_SRCS}/core/sdf-error-estimation.cpp
    ${MSDFGEN_SRCS}/core/Shape.cpp
    ${MSDFGEN_SRCS}/core/shape-description.cpp
    ${MSDFGEN_SRCS}/core/SignedDistance.cpp
    ${MSDFGEN_SRCS}/core/Vector2.cpp
    ${MSDFGEN_SRCS}/ext/import-font.cpp
    ${MSDFGEN_SRCS}/ext/resolve-shape-geometry.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/bitmap-blit.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/Charset.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/charset-parser.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/glyph-generators.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/GlyphGeometry.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/RectanglePacker.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/size-selectors.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/TightAtlasPacker.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/utf8.cpp
    ${MSDF_ATLAS_SRCS}/msdf-atlas-gen/Workload.cpp)

ze_add_private_modules(SDFFontGen Core GfxCore)

target_compile_definitions(SDFFontGen PRIVATE MSDFGEN_CMAKE_BUILD MSDFGEN_USE_OPENMP)
target_include_directories(SDFFontGen
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${MSDFGEN_SRCS}
        ${MSDFGEN_SRCS}/core
        ${MSDFGEN_SRCS}/ext
        ${MSDF_ATLAS_SRCS}/msdf-atlas-gen)
target_link_libraries(SDFFontGen PRIVATE freetype)