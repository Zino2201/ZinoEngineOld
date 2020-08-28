#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

CVulkanQueue::CVulkanQueue(CVulkanDevice& InDevice,
	const uint32_t& InFamilyIdx) : CVulkanDeviceResource(InDevice),
	FamilyIndex(InFamilyIdx)
{
	Queue = Device.GetDevice().getQueue(InFamilyIdx, 0);
}

CVulkanQueue::~CVulkanQueue() {}

void CVulkanQueue::Submit(CVulkanCommandBuffer* InCommandBuffer,
	const std::vector<vk::Semaphore>& InSignalSemaphores)
{
	vk::SubmitInfo SubmitInfo;

	SubmitInfo.setCommandBufferCount(1);
	SubmitInfo.setPCommandBuffers(&InCommandBuffer->GetCommandBuffer());
	SubmitInfo.setSignalSemaphoreCount(static_cast<uint32_t>(InSignalSemaphores.size()));
	SubmitInfo.setPSignalSemaphores(InSignalSemaphores.data());

	/**
	 * If command buffer has any wait semaphores
	 * pass it to submit info
	 */
	std::vector<vk::Semaphore> WaitSemaphores;
	if(!InCommandBuffer->WaitSemaphores.empty())
	{
		SubmitInfo.setWaitSemaphoreCount(
			static_cast<uint32_t>(InCommandBuffer->WaitSemaphores.size()));
		SubmitInfo.setPWaitSemaphores(InCommandBuffer->WaitSemaphores.data());
		SubmitInfo.setPWaitDstStageMask(InCommandBuffer->WaitFlags.data());
	}
	
	Queue.submit(1, &SubmitInfo, *InCommandBuffer->Fence);

	/** Mark fence as submitted */
	InCommandBuffer->bHasBeenSubmitted = true;
}