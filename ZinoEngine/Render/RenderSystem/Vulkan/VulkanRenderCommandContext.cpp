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
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

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

		IsBoundMap[InSet] = false;

		if(!DescriptorSets[InSet])
			LOG(ELogSeverity::Fatal, "Failed to allocate descriptor set")
	}

	return DescriptorSets[InSet];
}

void CVulkanRenderCommandContextPipelineSetManager::ResetSet(const uint32_t& InSet)
{
	DescriptorSets.erase(InSet);
	IsBoundMap.erase(InSet);
}

void CVulkanRenderCommandContextPipelineSetManager::Reset()
{
	DescriptorSets.clear();
	IsBoundMap.clear();
}

PFN_vkCmdBeginDebugUtilsLabelEXT BeginMarker;
PFN_vkCmdEndDebugUtilsLabelEXT EndMarker;

CVulkanRenderCommandContext::CVulkanRenderCommandContext(CVulkanDevice* InDevice,
	CVulkanQueue* InQueue)
	: Device(InDevice), Queue(InQueue), SetManager(this)
{
	/*std::vector<vk::DescriptorSetLayoutBinding> Bindings =
	{
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1,
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
	};

	GlobalSetLayout[0] = Device->GetDevice().createDescriptorSetLayoutUnique(
			vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(),
				static_cast<uint32_t>(Bindings.size()),
				Bindings.data())).value;
	std::vector<vk::DescriptorPoolSize> PoolSizes = 
	{ 
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1) 
	};

	GlobalPoolManager = std::make_unique<CVulkanDescriptorPoolManager>(
		42,
		PoolSizes,
		GlobalSetLayout);*/

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
	SetManager.Reset();
	CommandBufferManager->GetMainCommandBuffer()->EndRenderPass();

	CurrentPipeline = nullptr;
	CurrentPipelineLayout = nullptr;

	if (g_VulkanEnableValidationLayers)
	{
		EndMarker(static_cast<VkCommandBuffer>(CommandBufferManager->GetMainCommandBuffer()
			->GetCommandBuffer()));
	}
}

void CVulkanRenderCommandContext::BindGraphicsPipeline(
	IRenderSystemGraphicsPipeline* InGraphicsPipeline)
{
	CVulkanGraphicsPipeline* GraphicsPipeline =
		static_cast<CVulkanGraphicsPipeline*>(InGraphicsPipeline);

	if(CurrentPipeline != GraphicsPipeline)
	{
		CurrentPipeline = GraphicsPipeline;

		if(CurrentPipelineLayout != GraphicsPipeline->GetPipelineLayout())
		{
			SetManager.Reset(); 
			CurrentPipelineLayout = GraphicsPipeline->GetPipelineLayout();
		}

		CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
			.bindPipeline(vk::PipelineBindPoint::eGraphics, GraphicsPipeline->GetPipeline());

		/** Bind write sets that need to be bound */
		for(auto& WriteSet : WriteDescriptorSets)
		{
			vk::DescriptorSet Set = SetManager.GetDescriptorSet(0);
			WriteSet.Write.setDstSet(Set);

			switch(WriteSet.Write.descriptorType)
			{
			case vk::DescriptorType::eUniformBuffer:
				WriteSet.Write.setPBufferInfo(&WriteSet.BufferInfo);
				break;
			case vk::DescriptorType::eCombinedImageSampler:
				WriteSet.Write.setPImageInfo(&WriteSet.ImageInfo);
				break;
			default:
				must(true); // Not supported
				break;
			}

			g_VulkanRenderSystem->GetDevice()->GetDevice()
				.updateDescriptorSets(1, &WriteSet.Write, 0, nullptr);
		}

		WriteDescriptorSets.clear();
	}
}

void CVulkanRenderCommandContext::BindVertexBuffers(
	const std::vector<IRenderSystemVertexBuffer*>& InVertexBuffers)
{
	int Idx = 0;

	for (const auto& VertexBuffer : InVertexBuffers)
	{
		vk::Buffer Buffer = static_cast<CVulkanBuffer*>(
			static_cast<CVulkanVertexBuffer*>(VertexBuffer)->GetBuffer())->GetBuffer();

		CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().bindVertexBuffers(
			Idx,
			Buffer,
			{ 0 });

		Idx++;
	}
}

void CVulkanRenderCommandContext::BindIndexBuffer(IRenderSystemIndexBuffer* InIndexBuffer,
	const uint64_t& InOffset,
	const EIndexFormat& InIndexFormat)
{
	vk::Buffer Buffer = static_cast<CVulkanBuffer*>(
		static_cast<CVulkanIndexBuffer*>(InIndexBuffer)->GetBuffer())->GetBuffer();

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().bindIndexBuffer(
		Buffer,
		InOffset,
		VulkanUtil::IndexFormatToVkIndexType(InIndexFormat));
}

void CVulkanRenderCommandContext::SetShaderUniformBuffer(const uint32_t& InSet,
	const uint32_t& InBinding, IRenderSystemUniformBuffer* InUBO)
{
	/** If no pipeline layout is bound, agglomerate write informations into a
	 * array to apply them later (when a pipeline is being bound) */

	/**
	 * If set is bound, reset it
	 */
	if (SetManager.IsBound(InSet))
		SetManager.ResetSet(InSet);

	vk::DescriptorSet Set = nullptr;
	if(CurrentPipelineLayout)
		Set = SetManager.GetDescriptorSet(InSet);

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

	if(!CurrentPipelineLayout)
	{
		must(InSet == 0); // TODO: Need just to support multiple sets

		SVulkanWriteSet Write;
		Write.BufferInfo = BufferInfo;
		Write.Write = WriteSet;
		WriteDescriptorSets.emplace_back(Write);
	}
	else
		g_VulkanRenderSystem->GetDevice()->GetDevice()
			.updateDescriptorSets(1, &WriteSet, 0, nullptr);
}

void CVulkanRenderCommandContext::SetShaderCombinedImageSampler(const uint32_t& InSet,
	const uint32_t& InBinding, CRenderSystemTextureView* InView)
{
	if (SetManager.IsBound(InSet))
		SetManager.ResetBoundState(InSet);

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

	if (!CurrentPipelineLayout)
	{
		must(InSet == 0); // TODO: Need just to support multiple sets

		SVulkanWriteSet Write;
		Write.ImageInfo = ImageInfo;
		Write.Write = WriteSet;
		WriteDescriptorSets.push_back(Write);
	}
	else
		g_VulkanRenderSystem->GetDevice()->GetDevice()
			.updateDescriptorSets(1, &WriteSet, 0, nullptr);
}

void CVulkanRenderCommandContext::SetShaderStorageBuffer(const uint32_t& InSet,
	const uint32_t& InBinding, CRenderSystemBuffer* InBuffer)
{
	/** If no pipeline layout is bound, agglomerate write informations into a
	 * array to apply them later (when a pipeline is being bound) */

	/**
	 * If set is bound, reset it
	 */
	if (SetManager.IsBound(InSet))
		SetManager.ResetSet(InSet);

	vk::DescriptorSet Set = nullptr;
	if(CurrentPipelineLayout)
		Set = SetManager.GetDescriptorSet(InSet);

	vk::Buffer Buffer = static_cast<CVulkanBuffer*>(InBuffer)->GetBuffer();

	vk::DescriptorBufferInfo BufferInfo = vk::DescriptorBufferInfo()
		.setBuffer(Buffer)
		.setOffset(0)
		.setRange(VK_WHOLE_SIZE);

	vk::WriteDescriptorSet WriteSet = vk::WriteDescriptorSet()
		.setDstSet(Set)
		.setDstBinding(InBinding)
		.setDstArrayElement(0)
		.setDescriptorType(vk::DescriptorType::eStorageBuffer)
		.setDescriptorCount(1)
		.setPBufferInfo(&BufferInfo);

	if(!CurrentPipelineLayout)
	{
		must(InSet == 0); // TODO: Need just to support multiple sets

		SVulkanWriteSet Write;
		Write.BufferInfo = BufferInfo;
		Write.Write = WriteSet;
		WriteDescriptorSets.emplace_back(Write);
	}
	else
		g_VulkanRenderSystem->GetDevice()->GetDevice()
			.updateDescriptorSets(1, &WriteSet, 0, nullptr);
}

void CVulkanRenderCommandContext::BindDescriptorSets()
{
	for (const auto& [Set, DescriptorSet] : SetManager.GetDescriptorSets())
	{
		SetManager.MarkAsBound(Set);

		CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer()
			.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
				CurrentPipelineLayout->GetPipelineLayout(),
				Set,
				{ DescriptorSet },
				{});
	}
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

void CVulkanRenderCommandContext::SetViewports(const std::vector<SViewport>& InViewports)
{
	std::vector<vk::Viewport> Viewports;
	Viewports.reserve(InViewports.size());

	for (const auto& Viewport : InViewports)
	{
		Viewports.push_back(vk::Viewport(
			Viewport.Rect.Position.x,
			Viewport.Rect.Position.y,
			Viewport.Rect.Size.x,
			Viewport.Rect.Size.y,
			Viewport.MinDepth,
			Viewport.MaxDepth));
	}

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().setViewport(
		0, Viewports);
}

void CVulkanRenderCommandContext::SetScissors(const std::vector<SRect2D>& InScissors)
{
	std::vector<vk::Rect2D> Scissors;
	Scissors.reserve(InScissors.size());

	for(const auto& Scissor : InScissors)
	{
		Scissors.push_back(vk::Rect2D(vk::Offset2D(Scissor.Position.x,
			Scissor.Position.y), vk::Extent2D(Scissor.Size.x, Scissor.Size.y)));
	}

	CommandBufferManager->GetMainCommandBuffer()->GetCommandBuffer().setScissor(
		0, Scissors);
}