#pragma once

#include "VulkanCore.h"

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
	
	CVulkanCommandBuffer* GetMainCommandBuffer() const { return MainCommandBuffer.get(); }
private:
	CVulkanDevice* Device;
	std::unique_ptr<CVulkanCommandPool> CommandPool;
	std::unique_ptr<CVulkanCommandBuffer> MainCommandBuffer;
	CVulkanQueue* Queue;
};