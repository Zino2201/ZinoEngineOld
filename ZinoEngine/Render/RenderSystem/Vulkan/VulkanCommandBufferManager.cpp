#include "VulkanCommandBufferManager.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderCommandContext.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"

CVulkanCommandBufferManager::CVulkanCommandBufferManager(CVulkanDevice* InDevice,
	CVulkanRenderCommandContext* InContext) : Device(InDevice), Queue(InContext->GetQueue())
{
	/** Create pools */
	for(uint32_t i = 0; i < g_VulkanRenderSystem->GetSwapChain()->GetImageCount(); ++i)
		CommandPools.emplace_back(
			std::make_unique<CVulkanCommandPool>(Device, Queue->GetFamilyIndex()));

	/** Create command buffers */
	for (const std::unique_ptr<CVulkanCommandPool>& CommandPool : CommandPools)
	{
		CommandBuffers.emplace_back(std::make_unique<CVulkanCommandBuffer>(Device, CommandPool.get()));
	}
}

CVulkanCommandBufferManager::~CVulkanCommandBufferManager() {}

void CVulkanCommandBufferManager::SubmitMainCommandBuffer(const vk::Semaphore& InSignalSemaphore)
{
	Queue->Submit(CommandBuffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()].get(), 
		{ InSignalSemaphore },
		g_VulkanRenderSystem->GetSwapChain()->GetFenceForCurrentFrame());
}

CVulkanCommandBuffer* CVulkanCommandBufferManager::GetMainCommandBuffer() const
{
	return CommandBuffers[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()].get();
}