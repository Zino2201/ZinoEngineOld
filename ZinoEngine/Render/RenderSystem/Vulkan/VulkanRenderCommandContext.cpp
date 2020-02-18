#include "VulkanRenderCommandContext.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBufferManager.h"
#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"
#include "VulkanPipelineLayout.h"
#include "VulkanTextureView.h"
#include "VulkanSampler.h"

CVulkanRenderCommandContextPipelineSetManager::CVulkanRenderCommandContextPipelineSetManager(
	CVulkanRenderCommandContext* InCtxt) : Context(InCtxt) {}

vk::DescriptorSet CVulkanRenderCommandContextPipelineSetManager::GetDescriptorSet(
	const uint32_t& InSet)
{
	if (!DescriptorSets.count(InSet))
	{
		DescriptorSets[InSet] =
			Context->CurrentPipeline->GetPipelineLayout()->GetDescriptorPoolManager()
			->AllocateSet(InSet);

		if(!DescriptorSets[InSet])
			LOG(ELogSeverity::Fatal, "Failed to allocate descriptor set")
	}

	return DescriptorSets[InSet];
}

void CVulkanRenderCommandContextPipelineSetManager::Reset()
{
	DescriptorSets.clear();
}

PFN_vkCmdBeginDebugUtilsLabelEXT BeginMarker;
PFN_vkCmdEndDebugUtilsLabelEXT EndMarker;

CVulkanRenderCommandContext::CVulkanRenderCommandContext(CVulkanDevice* InDevice,
	CVulkanQueue* InQueue)
	: Device(InDevice), Queue(InQueue), SetManager(this)
{
	CommandBufferManager = std::make_unique<CVulkanCommandBufferManager>(Device, this);

	if (g_VulkanEnableValidationLayers)
	{
		BeginMarker = (PFN_vkCmdBeginDebugUtilsLabelEXT)
			g_VulkanRenderSystem->GetInstance().getProcAddr("vkCmdBeginDebugUtilsLabelEXT");

		EndMarker = (PFN_vkCmdEndDebugUtilsLabelEXT)
			g_VulkanRenderSystem->GetInstance().getProcAddr("vkCmdEndDebugUtilsLabelEXT");
	}
}

void CVulkanRenderCommandContext::Begin()
{
	CommandBufferManager->GetMainCommandBuffer()->Begin();
}

void CVulkanRenderCommandContext::End()
{
	CommandBufferManager->GetMainCommandBuffer()->End();
}

void CVulkanRenderCommandContext::BeginRenderPass(const std::array<float, 4>& InClearColor,
	const std::string& InName)
{
	CommandBufferManager->GetMainCommandBuffer()->BeginRenderPass(
		g_VulkanRenderSystem->GetRenderPass(),
		*g_VulkanRenderSystem->GetFramebuffers()[g_VulkanRenderSystem->GetSwapChain()->GetCurrentImageIndex()],
		InClearColor);

	if (g_VulkanEnableValidationLayers)
	{
		VkDebugUtilsLabelEXT LabelInfo = {};
		LabelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		LabelInfo.pLabelName = InName.c_str();

		BeginMarker(
			static_cast<VkCommandBuffer>(CommandBufferManager->GetMainCommandBuffer()
				->GetCommandBuffer()), &LabelInfo);
	}
}

void CVulkanRenderCommandContext::EndRenderPass()
{
	CommandBufferManager->GetMainCommandBuffer()->EndRenderPass();

	if (g_VulkanEnableValidationLayers)
	{
		EndMarker(static_cast<VkCommandBuffer>(CommandBufferManager->GetMainCommandBuffer()
			->GetCommandBuffer()));
	}
}

void CVulkanRenderCommandContext::BindGraphicsPipeline(IRenderSystemGraphicsPipeline* InGraphicsPipeline)
{
	CVulkanGraphicsPipeline* GraphicsPipeline =
		static_cast<CVulkanGraphicsPipeline*>(InGraphicsPipeline);

	CurrentPipeline = GraphicsPipeline;

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
		.bindPipeline(vk::PipelineBindPoint::eGraphics, GraphicsPipeline->GetPipeline());
}

void CVulkanRenderCommandContext::BindVertexBuffers(
	const std::vector<CRenderSystemBuffer*>& InVertexBuffers)
{
	for (const auto& VertexBuffer : InVertexBuffers)
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

void CVulkanRenderCommandContext::SetShaderUniformBuffer(const uint32_t& InSet,
	const uint32_t& InBinding, IRenderSystemUniformBuffer* InUBO)
{
	vk::DescriptorSet Set = SetManager.GetDescriptorSet(InSet);

	CVulkanBuffer* Buffer = static_cast<CVulkanBuffer*>(InUBO->GetBuffer());

	vk::DescriptorBufferInfo BufferInfo = vk::DescriptorBufferInfo()
		.setBuffer(Buffer->GetBuffer())
		.setOffset(0)
		.setRange(VK_WHOLE_SIZE);

	vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
		.setDstSet(Set)
		.setDstBinding(InBinding)
		.setDstArrayElement(0)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(1)
		.setPBufferInfo(&BufferInfo);

	g_VulkanRenderSystem->GetDevice()->GetDevice()
		.updateDescriptorSets(1, &WriteSet, 0, nullptr);
}

void CVulkanRenderCommandContext::SetShaderCombinedImageSampler(const uint32_t& InSet,
	const uint32_t& InBinding, CRenderSystemTextureView* InView)
{
	vk::DescriptorSet Set = SetManager.GetDescriptorSet(InSet);

	CVulkanTextureView* TextureView = static_cast<CVulkanTextureView*>(InView);

	vk::DescriptorImageInfo ImageInfo = vk::DescriptorImageInfo()
		.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
		.setImageView(TextureView->GetImageView())
		.setSampler(static_cast<CVulkanSampler*>(TextureView->GetSampler())->GetSampler());

	vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
		.setDstSet(Set)
		.setDstBinding(InBinding)
		.setDstArrayElement(0)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setDescriptorCount(1)
		.setPImageInfo(&ImageInfo);

	g_VulkanRenderSystem->GetDevice()->GetDevice()
		.updateDescriptorSets(1, &WriteSet, 0, nullptr);
}

void CVulkanRenderCommandContext::BindDescriptorSets()
{
	for (const auto& [Set, DescriptorSet] : SetManager.GetDescriptorSets())
	{
		CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
			.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
				CurrentPipeline->GetPipelineLayout()->GetPipelineLayout(),
				Set,
				{ DescriptorSet },
				{});
	}

	SetManager.Reset();
}

void CVulkanRenderCommandContext::Draw(const uint32_t& InVertexCount, const uint32_t& InInstanceCount,
	const uint32_t& InFirstVertex, const uint32_t& InFirstInstance)
{
	BindDescriptorSets();

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().draw(InVertexCount, InInstanceCount,
		InFirstVertex, InFirstInstance);
}

void CVulkanRenderCommandContext::DrawIndexed(const uint32_t& InIndexCount,
	const uint32_t& InInstanceCount, const uint32_t& InFirstIndex,
	const int32_t& InVertexOffset, const uint32_t& InFirstInstance)
{
	BindDescriptorSets();

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().drawIndexed(InIndexCount,
		InInstanceCount, InFirstIndex, InVertexOffset, InFirstInstance);
}
