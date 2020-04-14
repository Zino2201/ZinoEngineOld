#pragma once

#include "VulkanCore.h"
#include "VulkanRenderSystemContext.h"
#include "VulkanPipelineLayout.h"

class CVulkanSwapChain;
class CVulkanQueue;

/**
 * Staging buffer manager
 * Keeps track of staging buffers
 */
class CVulkanStagingBufferManager
{
public:
    CVulkanStagingBufferManager(CVulkanDevice* InDevice);

	CVulkanInternalStagingBuffer* CreateStagingBuffer(uint64_t InSize,
		vk::BufferUsageFlags InUsageFlags);
    void ReleaseStagingBuffer(CVulkanInternalStagingBuffer* InBuffer);
    void ReleaseStagingBuffers();
private:
    CVulkanDevice* Device;
    TSet<CVulkanInternalStagingBuffer*> StagingBuffers;
    std::vector<CVulkanInternalStagingBuffer*> StagingBuffersToDelete;
};

class CVulkanDevice
{
public:
    CVulkanDevice(const vk::PhysicalDevice& InPhysicalDevice);
    ~CVulkanDevice();

    void WaitIdle();
    void CreatePresentQueue(const vk::SurfaceKHR& InSurface);

    CVulkanStagingBufferManager* GetStagingBufferMgr() { return StagingBufferMgr.get(); }
    CVulkanPipelineLayoutManager* GetPipelineLayoutMgr() { return PipelineLayoutMgr.get(); }
    CVulkanRenderSystemContext* GetContext() const { return Context.get(); }
    const VmaAllocator& GetAllocator() const { return Allocator; }
    const vk::Device& GetDevice() const { return *Device; }
    const vk::PhysicalDevice& GetPhysicalDevice() const { return PhysicalDevice; }
    CVulkanQueue* GetGraphicsQueue() const { return GraphicsQueue.get(); }
    CVulkanQueue* GetPresentQueue() const { return PresentQueue; }
private:
    std::unique_ptr<CVulkanRenderSystemContext> Context;
    std::unique_ptr<CVulkanStagingBufferManager> StagingBufferMgr;
    VmaAllocator Allocator;
    vk::UniqueDevice Device;
    vk::PhysicalDevice PhysicalDevice;
    std::unique_ptr<CVulkanSwapChain> SwapChain;
    SVulkanQueueFamilyIndices QueueFamilyIndices;
    std::unique_ptr<CVulkanQueue> GraphicsQueue;
    CVulkanQueue* PresentQueue;
    std::unique_ptr<CVulkanPipelineLayoutManager> PipelineLayoutMgr;
};