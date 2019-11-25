#include "VulkanRenderCommandContext.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipeline.h"

CVulkanRenderCommandContext::CVulkanRenderCommandContext()
	: CommandPool(std::make_unique<CVulkanCommandPool>(g_VulkanRenderSystem->GetDevice())), 
	CommandBuffer(std::make_unique<CVulkanCommandBuffer>(g_VulkanRenderSystem->GetDevice(), 
		CommandPool.get()))
{

}

void CVulkanRenderCommandContext::Begin()
{
	CommandBuffer->Begin();
}

void CVulkanRenderCommandContext::End()
{
	CommandBuffer->End();
}

void CVulkanRenderCommandContext::BeginRenderPass(const std::array<float, 4>& InClearColor)
{
	CommandBuffer->BeginRenderPass(
		g_VulkanRenderSystem->GetRenderPass(),
		g_VulkanRenderSystem->GetCurrentFramebuffer(),
		InClearColor);
}

void CVulkanRenderCommandContext::EndRenderPass()
{
	CommandBuffer->EndRenderPass();
}

void CVulkanRenderCommandContext::BindGraphicsPipeline(IGraphicsPipeline* InGraphicsPipeline)
{
	CVulkanGraphicsPipeline* GraphicsPipeline = 
		static_cast<CVulkanGraphicsPipeline*>(InGraphicsPipeline);

	CommandBuffer->GetCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics,
		GraphicsPipeline->GetPipeline());
}

void CVulkanRenderCommandContext::Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
	const uint32_t& InFirstVertex, const uint32_t& InFirstInstance)
{
	CommandBuffer->GetCommandBuffer().draw(InVertexCount, InInstanceCount,
		InFirstVertex, InFirstInstance);
}
