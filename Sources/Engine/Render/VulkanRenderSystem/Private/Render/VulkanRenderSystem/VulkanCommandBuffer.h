#pragma once

#include "VulkanCore.h"

/**
 * A command buffer
 */
class CVulkanCommandBuffer : public CVulkanDeviceResource
{
    friend class CVulkanQueue;

public:
    CVulkanCommandBuffer(CVulkanDevice* InDevice,
        const vk::CommandPool& InParentPool,
        const bool& InFenceSignaledAtCreation);

    void Begin();
    void End();

    /**
     * Add a wait semaphore/flags
     */
    void AddWaitSemaphore(const vk::PipelineStageFlags& InStageFlags,
        const vk::Semaphore& InSemaphore);

    /**
     * Refresh fence status
     */
    void WaitFenceAndReset();

    const vk::CommandBuffer& GetCommandBuffer() const { return *CommandBuffer; }
    const bool& HasBeenSubmitted() const { return bHasBeenSubmitted; }
    const bool& HasBegun() const { return bHasBegun; }
private:
    /**
     * Command buffer handle
     */
    vk::UniqueCommandBuffer CommandBuffer;

    /**
     * Command buffer fence for synchronization
     */
    vk::UniqueFence Fence;

    /**
     * Command buffer current wait flags
     */
    std::vector<vk::PipelineStageFlags> WaitFlags;

    /**
     * Command buffer current semaphores (added by SwapChain every frame)
     */
    std::vector<vk::Semaphore> WaitSemaphores;

    bool bHasBeenSubmitted;
    bool bHasBegun;
};