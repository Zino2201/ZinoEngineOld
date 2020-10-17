#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"

CVulkanCommandBuffer::CVulkanCommandBuffer(CVulkanDevice& InDevice,
	const vk::CommandPool& InParentPool,
	const bool& InFenceSignaledAtCreation)
	: CVulkanDeviceResource(InDevice), bHasBegun(false)
{
	CommandBuffer = std::move(Device.GetDevice().allocateCommandBuffersUnique(
		vk::CommandBufferAllocateInfo(InParentPool,
			vk::CommandBufferLevel::ePrimary,
			1)).value.front());

	vk::FenceCreateFlags Flags;
	if(InFenceSignaledAtCreation)
		Flags |= vk::FenceCreateFlagBits::eSignaled;

	Fence = Device.GetDevice().createFenceUnique(
		vk::FenceCreateInfo(Flags)).value;
	if(!Fence)
		ze::logger::fatal("Failed to create fence");

	Begin();
}

void CVulkanCommandBuffer::Begin()
{
	bHasBegun = true;
	CommandBuffer->begin(
		vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
}

void CVulkanCommandBuffer::End()
{
	CommandBuffer->end();
	bHasBegun = false;
}

void CVulkanCommandBuffer::AddWaitSemaphore(const vk::PipelineStageFlags& InStageFlags,
	const vk::Semaphore& InSemaphore)
{
	WaitFlags.push_back(InStageFlags);
	WaitSemaphores.push_back(InSemaphore);
}

void CVulkanCommandBuffer::WaitFenceAndReset()
{
	if(!bHasBeenSubmitted)
		return;

	/**
	 * Wait for fence
	 */
	while(true)
	{
		vk::Result FenceStatus = Device.GetDevice().getFenceStatus(
			*Fence);

		/**
		 * If ready (signaled) then we can exit loop
		 */
		if(FenceStatus == vk::Result::eSuccess)
			break;
	}

	/** Clear pending staging buffers */
	Device.GetStagingBufferMgr().ReleaseStagingBuffers();

	/**
	 * Reset command buffer
	 */
	CommandBuffer->reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	/**
	 * Reset fence
	 */
	Device.GetDevice().resetFences({ *Fence });

	bHasBeenSubmitted = false;
	bHasBegun = false;

	WaitFlags.resize(0);
	WaitSemaphores.resize(0);
}