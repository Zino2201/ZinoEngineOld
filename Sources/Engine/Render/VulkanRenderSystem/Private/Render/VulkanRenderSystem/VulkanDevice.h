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

/**
 * Class used to defer destruction to certain resources
 */
class CVulkanDeferredDestructionManager
{
public:
    enum class EHandleType
    {
        Buffer,
        Image,
        ImageView
    };
    
    struct SEntry
    {
        EHandleType Type;
        VmaAllocation Allocation;
        uint64_t Handle;

        SEntry(EHandleType InType,
            const VmaAllocation& InAllocation,
            uint64_t InHandle) : Type(InType),
            Allocation(InAllocation), Handle(InHandle) {}

		SEntry(EHandleType InType,
			uint64_t InHandle) : Type(InType),
			Handle(InHandle) {}
    };
    CVulkanDeferredDestructionManager(CVulkanDevice& InDevice) : Device(InDevice) {}

    void EnqueueImage(EHandleType InHandleType, const VmaAllocation& InAllocation, 
        vk::Image& InHandle)
    {
        EnqueueResource(InHandleType, InAllocation, 
            reinterpret_cast<uint64_t>(static_cast<VkImage>(InHandle)));
	    InHandle = vk::Image();
    }

	void EnqueueImageView(EHandleType InHandleType, vk::ImageView& InHandle)
	{
		EnqueueResource(InHandleType, reinterpret_cast<uint64_t>(static_cast<VkImageView>(InHandle)));
	    InHandle = vk::ImageView();
    }

    void DestroyResources();
private:    
    void EnqueueResource(EHandleType InHandleType, 
        const VmaAllocation& InAllocation, uint64_t InHandle);
	void EnqueueResource(EHandleType InHandleType,
		uint64_t InHandle);
private:
    CVulkanDevice& Device;
    std::vector<SEntry> Entries;
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
    CVulkanDeferredDestructionManager& GetDeferredDestructionMgr() { return DeferredDestructionManager; }
    CVulkanRenderSystemContext* GetContext() const { return Context.get(); }
    const VmaAllocator& GetAllocator() const { return Allocator; }
    const vk::Device& GetDevice() const { return *Device; }
    const vk::PhysicalDevice& GetPhysicalDevice() const { return PhysicalDevice; }
    CVulkanQueue* GetGraphicsQueue() const { return GraphicsQueue.get(); }
    CVulkanQueue* GetPresentQueue() const { return PresentQueue; }
private:
    CVulkanDeferredDestructionManager DeferredDestructionManager;
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