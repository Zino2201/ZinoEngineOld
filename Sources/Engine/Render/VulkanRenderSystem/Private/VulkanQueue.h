#pragma once

#include "VulkanCore.h"

class CVulkanCommandBuffer;

/**
 * An vulkan queue
 */
class CVulkanQueue : public CVulkanDeviceResource
{
public:
    CVulkanQueue(CVulkanDevice& InDevice, 
        const uint32_t& InFamilyIdx);
    ~CVulkanQueue();

    /**
     * Submit a command buffer to the queue
     */
    void Submit(CVulkanCommandBuffer* InCommandBuffer,
        const std::vector<vk::Semaphore>& InSignalSemaphores);

    const uint32_t& GetFamilyIndex() const { return FamilyIndex; }
    const vk::Queue& GetQueue() const { return Queue; }
private:
    vk::Queue Queue;
    uint32_t FamilyIndex;
};