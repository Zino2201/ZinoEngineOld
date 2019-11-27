#include "VulkanRenderCommandContext.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBufferManager.h"
#include "VulkanSwapChain.h"

CVulkanRenderCommandContext::CVulkanRenderCommandContext(CVulkanDevice* InDevice,
	CVulkanQueue* InQueue)
	: Device(InDevice), Queue(InQueue)
{
	CommandBufferManager = std::make_unique<CVulkanCommandBufferManager>(Device, this);
}

void CVulkanRenderCommandContext::Begin()
{
	CommandBufferManager->GetMainCommandBuffer()->Begin();
}

void CVulkanRenderCommandContext::End()
{
	CommandBufferManager->GetMainCommandBuffer()->End();
}

void CVulkanRenderCommandContext::BeginRenderPass(const std::array<float, 4>& InClearColor)
{
	CommandBufferManager->GetMainCommandBuffer()->BeginRenderPass(
		g_VulkanRenderSystem->GetRenderPass(),
		*g_VulkanRenderSystem->GetFramebuffers()[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()],
		InClearColor);
}

void CVulkanRenderCommandContext::EndRenderPass()
{
	CommandBufferManager->GetMainCommandBuffer()->EndRenderPass();
}

void CVulkanRenderCommandContext::BindGraphicsPipeline(IGraphicsPipeline* InGraphicsPipeline)
{
	CVulkanGraphicsPipeline* GraphicsPipeline = 
		static_cast<CVulkanGraphicsPipeline*>(InGraphicsPipeline);

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics,
		GraphicsPipeline->GetPipeline());
}

void CVulkanRenderCommandContext::Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
	const uint32_t& InFirstVertex, const uint32_t& InFirstInstance)
{
	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().draw(InVertexCount, InInstanceCount,
		InFirstVertex, InFirstInstance);
}
