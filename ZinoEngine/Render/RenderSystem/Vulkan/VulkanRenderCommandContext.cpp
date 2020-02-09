#include "VulkanRenderCommandContext.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBufferManager.h"
#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"
#include "VulkanShaderAttributesManager.h"
#include "VulkanPipelineLayout.h"

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

void CVulkanRenderCommandContext::BindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline)
{
	CVulkanGraphicsPipeline* GraphicsPipeline = 
		static_cast<CVulkanGraphicsPipeline*>(InGraphicsPipeline);

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
		.bindPipeline(vk::PipelineBindPoint::eGraphics, GraphicsPipeline->GetPipeline());
}

void CVulkanRenderCommandContext::BindShaderAttributesManager(
	const std::shared_ptr<IShaderAttributesManager>& InManager)
{
	CVulkanShaderAttributesManager* ShaderAttributesManager =
		static_cast<CVulkanShaderAttributesManager*>(InManager.get());

	//CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
	//	.pushConstants()

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
		.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			static_cast<CVulkanPipeline*>(InManager->GetInfos().Pipeline)
			->GetPipelineLayout()->GetPipelineLayout(),
			static_cast<uint8_t>(InManager->GetInfos().Frequency),
			{ *ShaderAttributesManager->GetDescriptorSets()[0] },
			{});
}

void CVulkanRenderCommandContext::BindVertexBuffers(
	const std::vector<CRenderSystemBuffer*>& InVertexBuffers)
{
	for(const auto& VertexBuffer : InVertexBuffers)
	{
		vk::Buffer Buffer = static_cast<CVulkanBuffer*>(VertexBuffer)->GetBuffer();

		CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().bindVertexBuffers(
			0,
			Buffer,
			{ 0 });
	}
}

void CVulkanRenderCommandContext::BindIndexBuffer(CRenderSystemBuffer* InIndexBuffer,
	const uint64_t& InOffset,
	const EIndexFormat& InIndexFormat)
{
	vk::Buffer Buffer = static_cast<CVulkanBuffer*>(InIndexBuffer)->GetBuffer();

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().bindIndexBuffer(
		Buffer,
		InOffset,
		VulkanUtil::IndexFormatToVkIndexType(InIndexFormat));
}

void CVulkanRenderCommandContext::Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
	const uint32_t& InFirstVertex, const uint32_t& InFirstInstance)
{
	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().draw(InVertexCount, InInstanceCount,
		InFirstVertex, InFirstInstance);
}

void CVulkanRenderCommandContext::DrawIndexed(const uint32_t& InIndexCount, 
	const uint32_t& InInstanceCount,const uint32_t& InFirstIndex, 
	const int32_t& InVertexOffset, const uint32_t& InFirstInstance)
{
	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().drawIndexed(InIndexCount,
		InInstanceCount, InFirstIndex, InVertexOffset, InFirstInstance);
}