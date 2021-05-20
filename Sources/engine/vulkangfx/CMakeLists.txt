find_package(Vulkan REQUIRED)

add_library(vulkangfx
    public/Gfx/Vulkan/Backend.h
    private/Command.h
    private/DescriptorSet.h
    private/Device.h
    private/Pipeline.h
    private/PipelineLayout.h
    private/Queue.h
    private/RenderPass.h
    private/Shader.h
    private/SwapChain.h
    private/Sync.h
    private/Texture.h
    private/Vulkan.h
    private/VulkanBackend.h
    private/VulkanUtil.h
    private/VulkanBackend.cpp
    private/Buffer.cpp
    private/Device.cpp
    private/Queue.cpp
    private/SwapChain.cpp
    private/Surface.cpp
    private/Texture.cpp
    private/Sync.cpp
    private/Command.cpp
    private/Pipeline.cpp
    private/Shader.cpp
    private/RenderPass.cpp
    private/PipelineLayout.cpp
    private/Commands.cpp
    private/DescriptorSet.cpp
    private/Sampler.cpp)

target_link_libraries(vulkangfx
    PRIVATE
        core
        gfxbackend
        Vulkan::Vulkan
        SDL2)

target_include_directories(vulkangfx
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${ZE_LIBS_DIR}/VulkanMemoryAllocator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private)