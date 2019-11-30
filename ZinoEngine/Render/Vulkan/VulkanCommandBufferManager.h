#pragma once

#include "VulkanCore.h"
#include "VulkanSwapChain.h"

class CVulkanDevice;
class CVulkanRenderCommandContext;
class CVulkanCommandBuffer;
class CVulkanCommandPool;
class CVulkanQueue;

/**
 * Command buffer manager
 */
class CVulkanCommandBufferManager
{
public:
	CVulkanCommandBufferManager(CVulkanDevice* InDevice,
		CVulkanRenderCommandContext* InContext);
	~CVulkanCommandBufferManager();

	/** 
	 * Submit main command buffer
	 */
	void SubmitMainCommandBuffer(const vk::Semaphore& InSignalSemaphore);
	
	CVulkanCommandBuffer* GetMainCommandBuffer() const;
	const std::vector<std::unique_ptr<CVulkanCommandPool>>& GetCommandPools() const { return CommandPools; }
private:
	CVulkanDevice* Device;

	/**
	 * Commands pools
	 * One per frame per thread
	 */
	std::vector<std::unique_ptr<CVulkanCommandPool>> CommandPools;

	/**
	 * Command buffers
	 * One per pool
	 */
	std::vector<std::unique_ptr<CVulkanCommandBuffer>> CommandBuffers;

	CVulkanQueue* Queue;
};