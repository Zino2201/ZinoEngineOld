#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

CVulkanQueue::CVulkanQueue(CVulkanDevice* InDevice, uint32_t InFamilyIndex)
	: CVulkanDeviceResource(InDevice)
{
	Queue = Device->GetDevice().getQueue(InFamilyIndex, 0);
}

CVulkanQueue::~CVulkanQueue() {}

void CVulkanQueue::Submit(CVulkanCommandBuffer* InCommandBuffer,
	const std::vector<vk::Semaphore>& InSignalSemaphores)
{
	vk::SubmitInfo SubmitInfo(
		static_cast<uint32_t>(InCommandBuffer->GetWaitSemaphores().size()),
		InCommandBuffer->GetWaitSemaphores().data(),
		&InCommandBuffer->GetWaitFlags(),
		1,
		&InCommandBuffer->GetCommandBuffer(),
		static_cast<uint32_t>(InSignalSemaphores.size()),
		InSignalSemaphores.data());
	
	Queue.submit(1, &SubmitInfo, vk::Fence());

	InCommandBuffer->SubmitSemaphores();
}