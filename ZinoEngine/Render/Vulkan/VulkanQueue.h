#pragma once

#include "VulkanDeviceResource.h"

class CVulkanCommandBuffer;

class CVulkanQueue : public CVulkanDeviceResource
{
public:
	CVulkanQueue(CVulkanDevice* InDevice,
		uint32_t InFamilyIndex);
	~CVulkanQueue();

	/**
	 * Submit a command buffer
	 */
	void Submit(CVulkanCommandBuffer* InCommandBuffer,
		const std::vector<vk::Semaphore>& InSignalSemaphores);

	const vk::Queue& GetQueue() const { return Queue; }
private:
	vk::Queue Queue;
};