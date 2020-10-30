find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

ze_create_module(VulkanBackend 
    Public/Gfx/Vulkan/Backend.h
    Private/Buffer.h Private/Command.h Private/DescriptorSet.h Private/Device.h
    Private/Pipeline.h Private/PipelineLayout.h Private/Queue.h Private/RenderPass.h
    Private/Shader.h Private/SwapChain.h Private/Sync.h Private/Texture.h Private/Vulkan.h
    Private/VulkanBackend.h Private/VulkanUtil.h
    Private/VulkanBackend.cpp
    Private/Buffer.cpp Private/Device.cpp Private/Queue.cpp Private/SwapChain.cpp
    Private/Surface.cpp Private/Texture.cpp Private/Sync.cpp Private/Command.cpp
    Private/Pipeline.cpp Private/Shader.cpp Private/RenderPass.cpp Private/PipelineLayout.cpp
    Private/Commands.cpp Private/DescriptorSet.cpp Private/Sampler.cpp)
ze_add_public_modules(VulkanBackend GfxCore)

target_include_directories(VulkanBackend
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/VulkanMemoryAllocator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(VulkanBackend PRIVATE Vulkan::Vulkan SDL2::SDL2)