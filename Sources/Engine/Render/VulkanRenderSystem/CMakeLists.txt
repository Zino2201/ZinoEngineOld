find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

ze_create_module(VulkanRenderSystem Private/VulkanBuffer.cpp
    Private/VulkanCommandBuffer.cpp Private/VulkanDevice.cpp Private/VulkanMemoryAllocator.cpp
    Private/VulkanPipeline.cpp Private/VulkanPipelineLayout.cpp Private/VulkanQueue.cpp
    Private/VulkanRenderSystem.cpp Private/VulkanRenderSystemContext.cpp
    Private/VulkanSampler.cpp Private/VulkanShader.cpp Private/VulkanSurface.cpp
    Private/VulkanSwapChain.cpp Private/VulkanTexture.cpp)
ze_add_public_modules(VulkanRenderSystem Core ShaderCore RenderSystem RenderCore)

target_include_directories(VulkanRenderSystem
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
        ${ZE_LIBS_DIR}/VulkanMemoryAllocator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private)
target_link_libraries(VulkanRenderSystem PRIVATE Vulkan::Vulkan SDL2::SDL2)