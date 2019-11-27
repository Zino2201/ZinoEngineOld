#include "VulkanCommandBufferManager.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderCommandContext.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"

CVulkanCommandBufferManager::CVulkanCommandBufferManager(CVulkanDevice* InDevice,
	CVulkanRenderCommandContext* InContext) : Device(InDevice), Queue(InContext->GetQueue())
{
	/** Create pool */
	CommandPool = std::make_unique<CVulkanCommandPool>(Device, Queue->GetFamilyIndex());

	/** Create main command buffer */
	MainCommandBuffer = std::make_unique<CVulkanCommandBuffer>(Device, CommandPool.get());
}

CVulkanCommandBufferManager::~CVulkanCommandBufferManager() {}

void CVulkanCommandBufferManager::SubmitMainCommandBuffer(const vk::Semaphore& InSignalSemaphore)
{
	Queue->Submit(MainCommandBuffer.get(), { InSignalSemaphore });
}