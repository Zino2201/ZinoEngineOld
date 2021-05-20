add_library(vulkanshadercompiler private/VulkanShaderCompiler.cpp)
target_link_libraries(vulkanshadercompiler PRIVATE core zefs shadercompiler shadercore)

target_include_directories(vulkanshadercompiler
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private
        ${ZE_LIBS_DIR}/ShaderConductor/Include
        ${ZE_LIBS_DIR}/ShaderConductor/External/SPIRV-Cross)

if(${ZE_CURRENT_COMPILER} STREQUAL "ClangCL")
    if(ZE_MONOLITHIC)
        target_link_directories(vulkanshadercompiler
            PUBLIC 
                ${ZE_LIBS_DIR}/ShaderConductor/Build/vs2019-win-vc142-x64/Lib/${ZE_CONFIG_NAME_DBGREL})
    else()
        target_link_directories(vulkanshadercompiler
            PRIVATE 
                ${ZE_LIBS_DIR}/ShaderConductor/Build/vs2019-win-vc142-x64/Lib/${ZE_CONFIG_NAME_DBGREL})
    endif()
else()
    if(ZE_MONOLITHIC)
        target_link_directories(vulkanshadercompiler
            PUBLIC 
            ${ZE_LIBS_DIR}/ShaderConductor/Build/ninja-linux-clang-x64-${ZE_CONFIG_NAME_DBGREL}/Lib)
    else()
        target_link_directories(vulkanshadercompiler
            PRIVATE 
                ${ZE_LIBS_DIR}/ShaderConductor/Build/ninja-linux-clang-x64-${ZE_CONFIG_NAME_DBGREL}/Lib)
    endif()
endif()

target_link_libraries(vulkanshadercompiler PRIVATE ShaderConductor
    SPIRV-Tools SPIRV-Tools-opt "$<$<CONFIG:Debug>:spirv-cross-cored>$<$<CONFIG:RelWithDebInfo>:spirv-cross-core>$<$<CONFIG:Release>:spirv-cross-core>")