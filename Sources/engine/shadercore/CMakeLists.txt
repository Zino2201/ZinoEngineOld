add_library(shadercore private/shader/ShaderCore.cpp)
target_link_libraries(shadercore PUBLIC core)
target_include_directories(shadercore
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)