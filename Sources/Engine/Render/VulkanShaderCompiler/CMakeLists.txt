ze_create_module(VulkanShaderCompiler Private/VulkanShaderCompiler.cpp)
ze_add_public_modules(VulkanShaderCompiler Core ZEFS ShaderCore ShaderCompiler)

target_include_directories(VulkanShaderCompiler
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
        ${ZE_LIBS_DIR}/ShaderConductor/Include
        ${ZE_LIBS_DIR}/ShaderConductor/External/SPIRV-Cross)

if(${ZE_CURRENT_COMPILER} STREQUAL "ClangCL")
    if(ZE_MONOLITHIC)
        target_link_directories(VulkanShaderCompiler 
            PUBLIC 
                ${ZE_LIBS_DIR}/ShaderConductor/Build/vs2019-win-vc142-x64/Lib/${ZE_CONFIG_NAME_DBGREL})
    else()
        target_link_directories(VulkanShaderCompiler 
            PRIVATE 
                ${ZE_LIBS_DIR}/ShaderConductor/Build/vs2019-win-vc142-x64/Lib/${ZE_CONFIG_NAME_DBGREL})
    endif()
else()
    if(ZE_MONOLITHIC)
        target_link_directories(VulkanShaderCompiler 
            PUBLIC 
            ${ZE_LIBS_DIR}/ShaderConductor/Build/ninja-linux-clang-x64-${ZE_CONFIG_NAME_DBGREL}/Lib)
    else()
        target_link_directories(VulkanShaderCompiler 
            PRIVATE 
                ${ZE_LIBS_DIR}/ShaderConductor/Build/ninja-linux-clang-x64-${ZE_CONFIG_NAME_DBGREL}/Lib)
    endif()
endif()

target_link_libraries(VulkanShaderCompiler PRIVATE ShaderConductor 
    SPIRV-Tools SPIRV-Tools-opt "$<$<CONFIG:Debug>:spirv-cross-cored>$<$<CONFIG:RelWithDebInfo>:spirv-cross-core>$<$<CONFIG:Release>:spirv-cross-core>")