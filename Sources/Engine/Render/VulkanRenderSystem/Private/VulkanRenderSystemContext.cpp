#include "VulkanRenderSystemContext.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanSurface.h"
#include "VulkanTexture.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanSampler.h"

CVulkanCommandBufferManager::CVulkanCommandBufferManager(CVulkanDevice& InDevice)
	: Device(InDevice)
{
	CommandPool = Device.GetDevice().createCommandPoolUnique(
		vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlagBits::eTransient |
				vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		Device.GetGraphicsQueue()->GetFamilyIndex())).value;
	if(!CommandPool)
		ZE::Logger::Error("Failed to create command pool");

	/**
	 * Allocate 2 command buffers
	 */
	MemoryCmdBuffer = std::make_unique<CVulkanCommandBuffer>(Device, *CommandPool, false);
	if(!MemoryCmdBuffer)
		ZE::Logger::Fatal("Failed to allocate memory command buffer");

	GraphicsCmdBuffer = std::make_unique<CVulkanCommandBuffer>(Device, *CommandPool, false);
	if (!GraphicsCmdBuffer)
		ZE::Logger::Fatal("Failed to allocate graphics command buffer");
}

CVulkanCommandBufferManager::~CVulkanCommandBufferManager() = default;

void CVulkanCommandBufferManager::BeginMemoryCmdBuffer()
{
	/**
	 * If command buffer is submitted, submit and wait
	 */
	if(MemoryCmdBuffer->HasBegun())
	{
		SubmitMemoryCmdBuffer();
	}

	MemoryCmdBuffer->Begin();
}

void CVulkanCommandBufferManager::SubmitMemoryCmdBuffer()
{
	SubmitCmdBuffer(MemoryCmdBuffer.get(), nullptr);
	if (MemoryCmdBuffer->HasBeenSubmitted())
		MemoryCmdBuffer->WaitFenceAndReset();
}

void CVulkanCommandBufferManager::SubmitGraphicsCmdBuffer(const vk::Semaphore& InSemaphore)
{
	SubmitCmdBuffer(GraphicsCmdBuffer.get(), InSemaphore);

	// TODO: Change for inflight frames
	if (GraphicsCmdBuffer->HasBeenSubmitted())
		GraphicsCmdBuffer->WaitFenceAndReset();

	/** Release deferred destruction resources */
	Device.GetDeferredDestructionMgr().DestroyResources();

	GraphicsCmdBuffer->Begin();
}

void CVulkanCommandBufferManager::SubmitCmdBuffer(CVulkanCommandBuffer* InCommandBuffer,
	const vk::Semaphore& InSemaphore)
{
	if(InCommandBuffer->HasBegun())
	{
		InCommandBuffer->End();

		if (InSemaphore)
		{
			Device.GetGraphicsQueue()->Submit(InCommandBuffer, { InSemaphore });
		}
		else
		{
			Device.GetGraphicsQueue()->Submit(InCommandBuffer, {});
		}
	}

	/** Free staging buffers */

	/** Reset command buffers */
}

/** RENDER SYSTEM CONTEXT */

PFN_vkCmdBeginDebugUtilsLabelEXT BeginMarker;
PFN_vkCmdEndDebugUtilsLabelEXT EndMarker;

CVulkanRenderSystemContext::CVulkanRenderSystemContext(CVulkanDevice& InDevice)
	: Device(InDevice), CmdBufferMgr(InDevice), CurrentLayout(nullptr), ColorAttachmentsCount(0)
{
	GRenderSystemContext = this;
	GRSContext = this;

	if (GVulkanEnableValidationLayers)
	{
		BeginMarker = (PFN_vkCmdBeginDebugUtilsLabelEXT)
			GVulkanRenderSystem->GetInstance().getProcAddr("vkCmdBeginDebugUtilsLabelEXT");

		EndMarker = (PFN_vkCmdEndDebugUtilsLabelEXT)
			GVulkanRenderSystem->GetInstance().getProcAddr("vkCmdEndDebugUtilsLabelEXT");
	}
}

CVulkanRenderSystemContext::~CVulkanRenderSystemContext() { }

void CVulkanRenderSystemContext::BeginRenderPass(const SRSRenderPass& InRenderPass,
	const SRSFramebuffer& InFramebuffer,
	const std::array<float, 4>& InClearColor,
	const char* InName)
{
	vk::RenderPass RenderPass = Device.GetRenderPassFramebufferMgr().GetRenderPass(InRenderPass);
	vk::Framebuffer Framebuffer = Device.GetRenderPassFramebufferMgr().GetFramebuffer(InFramebuffer,
		RenderPass);

	ColorAttachmentsCount = static_cast<uint32_t>(InRenderPass.ColorAttachments.size());
	CurrentRenderPass = RenderPass;
	
	std::array<vk::ClearValue, 2> ClearValues =
	{
		vk::ClearColorValue(InClearColor),
		vk::ClearDepthStencilValue(0.f, 0)
	};

	vk::RenderPassBeginInfo BeginInfo = vk::RenderPassBeginInfo(
		RenderPass,
		Framebuffer,
		vk::Rect2D(vk::Offset2D(), 
			vk::Extent2D(InFramebuffer.ColorRTs[0]->GetCreateInfo().Width,
				InFramebuffer.ColorRTs[0]->GetCreateInfo().Height)),
		static_cast<uint32_t>(ClearValues.size()),
		ClearValues.data());

	if (GVulkanEnableValidationLayers)
	{
		VkDebugUtilsLabelEXT LabelInfo = {};
		LabelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		LabelInfo.pLabelName = InName;

		BeginMarker(
			static_cast<VkCommandBuffer>(CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()),
			&LabelInfo);
	}

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.beginRenderPass(BeginInfo, vk::SubpassContents::eInline);
}

void CVulkanRenderSystemContext::EndRenderPass()
{
	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer().endRenderPass();

	if (GVulkanEnableValidationLayers)
		EndMarker(static_cast<VkCommandBuffer>(CmdBufferMgr.GetGraphicsCmdBuffer()
			->GetCommandBuffer()));

	CurrentRenderPass = vk::RenderPass();
}

bool CVulkanRenderSystemContext::BeginSurface(CRSSurface* InSurface)
{
	must(InSurface);

	CVulkanSurface* Surface = static_cast<CVulkanSurface*>(InSurface);
	CurrentSurface = Surface;

	CurrentSurface->ResetOldSwapchain();

	return CurrentSurface->AcquireImage();
}

void CVulkanRenderSystemContext::PresentSurface(CRSSurface* InSurface)
{
	must(InSurface);

	CVulkanSurface* Surface = static_cast<CVulkanSurface*>(InSurface);
	must(Surface == CurrentSurface);

	/**
	 * Submit render queue if we have commands
	 */
	if(CmdBufferMgr.GetGraphicsCmdBuffer()->HasBegun())
	{
		CmdBufferMgr.GetGraphicsCmdBuffer()->AddWaitSemaphore(
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			Surface->GetSwapChain()->GetImageAcquiredSemaphore());

		CmdBufferMgr.SubmitGraphicsCmdBuffer(
			Surface->GetSwapChain()->GetRenderFinishedSemaphore());

		/**
		 * Present the surface
		 */
		Surface->Present(Device.GetPresentQueue());
	}

	CurrentSurface = nullptr;
}

void CVulkanRenderSystemContext::BindGraphicsPipeline(const SRSGraphicsPipeline& InGraphicsPipeline)
{
	/**
	 * Get or create a pipeline
	 */
	CVulkanGraphicsPipeline* Pipeline = Device.GetPipelineManager().GetOrCreateGraphicsPipeline(
		InGraphicsPipeline,
		CurrentRenderPass);

	if (CurrentLayout != Pipeline->GetPipelineLayout())
	{
		CurrentLayout = Pipeline->GetPipelineLayout();
	}

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.bindPipeline(vk::PipelineBindPoint::eGraphics, Pipeline->GetPipeline());
}

void CVulkanRenderSystemContext::SetViewports(const std::vector<SViewport>& InViewports)
{
	std::vector<vk::Viewport> Viewports;
	Viewports.reserve(InViewports.size());

	for (const auto& Viewport : InViewports)
	{
		Viewports.emplace_back(
			Viewport.Rect.Position.x,
			Viewport.Rect.Position.y,
			Viewport.Rect.Size.x,
			Viewport.Rect.Size.y,
			Viewport.MinDepth,
			Viewport.MaxDepth);
	}

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.setViewport(
		0, Viewports);
}

void CVulkanRenderSystemContext::SetScissors(const std::vector<SRect2D>& InScissors)
{
	std::vector<vk::Rect2D> Scissors;
	Scissors.reserve(InScissors.size());

	for (const auto& Scissor : InScissors)
	{
		Scissors.emplace_back(vk::Offset2D(
			static_cast<int32_t>(Scissor.Position.x),
			static_cast<int32_t>(Scissor.Position.y)), vk::Extent2D(
				static_cast<uint32_t>(Scissor.Size.x), static_cast<uint32_t>(Scissor.Size.y)));
	}

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.setScissor(
		0, Scissors);
}

void CVulkanRenderSystemContext::BindVertexBuffers(
	const std::vector<CRSBuffer*> InVertexBuffers)
{
	std::vector<vk::Buffer> Buffers;
	Buffers.reserve(InVertexBuffers.size());
	for(const auto& VertexBuffer : InVertexBuffers)
	{
		if(!VertexBuffer)
			return;

		Buffers.emplace_back(
			static_cast<CVulkanBuffer*>(VertexBuffer)->GetBuffer());
	}

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.bindVertexBuffers(0, Buffers, { 0 });
}

void CVulkanRenderSystemContext::BindIndexBuffer(CRSBuffer* InIndexBuffer,
	const uint64_t& InOffset,
	const EIndexFormat& InIndexFormat)
{
	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.bindIndexBuffer(static_cast<CVulkanBuffer*>(InIndexBuffer)->GetBuffer(),
			InOffset,
			InIndexFormat == EIndexFormat::Uint16 
				? vk::IndexType::eUint16 : vk::IndexType::eUint32);
}

void CVulkanRenderSystemContext::SetShaderUniformBuffer(const uint32_t& InSet, 
	const uint32_t& InBinding, CRSBuffer* InBuffer)
{
	verify(InBuffer);
	if (!InBuffer)
		return;

	CVulkanBuffer* Buffer = static_cast<CVulkanBuffer*>(InBuffer);

	vk::DescriptorBufferInfo BufferInfo = vk::DescriptorBufferInfo()
		.setBuffer(Buffer->GetBuffer())
		.setOffset(0)
		.setRange(VK_WHOLE_SIZE);

	SDescriptorSetWrite WriteSet(vk::DescriptorType::eUniformBuffer,
		BufferInfo, InBinding, 1);
	
	AddWrite(InSet, InBinding, WriteSet, 
		reinterpret_cast<uint64_t>(static_cast<VkBuffer>(Buffer->GetBuffer())));
}

void CVulkanRenderSystemContext::SetShaderTexture(const uint32_t& InSet, const uint32_t& InBinding,
	CRSTexture* InTexture)
{
	verify(InTexture);
	if(!InTexture)
		return;

	CVulkanTexture* Texture = static_cast<CVulkanTexture*>(InTexture);

	vk::ImageLayout ImgLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

	if(Texture->GetCreateInfo().Usage & ERSTextureUsageFlagBits::DepthStencil)
	{
		ImgLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
	}

	vk::DescriptorImageInfo ImageInfo = vk::DescriptorImageInfo()
		.setImageLayout(ImgLayout)
		.setImageView(Texture->GetImageView());

	SDescriptorSetWrite WriteSet(vk::DescriptorType::eSampledImage,
		ImageInfo, InBinding, 1);

	AddWrite(InSet, InBinding, WriteSet, 
		reinterpret_cast<uint64_t>(static_cast<VkImageView>(Texture->GetImageView())));
}

void CVulkanRenderSystemContext::SetShaderSampler(const uint32_t& InSet, const uint32_t& InBinding,
	CRSSampler* InSampler)
{
	verify(InSampler);
	if (!InSampler)
		return;

	CVulkanSampler* Sampler = static_cast<CVulkanSampler*>(InSampler);

	vk::DescriptorImageInfo ImageInfo = vk::DescriptorImageInfo()
		.setSampler(Sampler->GetSampler());

	SDescriptorSetWrite WriteSet(vk::DescriptorType::eSampler,
		ImageInfo, InBinding, 1);

	AddWrite(InSet, InBinding, WriteSet, 
		reinterpret_cast<uint64_t>(static_cast<VkSampler>(Sampler->GetSampler())));
}

void CVulkanRenderSystemContext::AddWrite(const uint32_t& InSet, const uint32_t& InBinding,
	const SDescriptorSetWrite& InWrite, const uint64_t& InHandle)
{
	verify(InBinding < GMaxBindingsPerSet);

	WriteMap[InSet][InBinding] = std::move(InWrite);
	HandleMap[InSet][InBinding] = InHandle;
}

void CVulkanRenderSystemContext::BindDescriptorSets()
{
	verify(CurrentLayout);

	for(const auto& [SetIdx, Writes] : WriteMap)
	{
		const auto& Handles = HandleMap[SetIdx];

		auto [Set, bMustUpdate] = CurrentLayout->GetSetManager().GetSet(SetIdx,
			Handles);

		if(bMustUpdate)
		{
			std::vector<vk::WriteDescriptorSet> FinalWrites;
			FinalWrites.reserve(Writes.size());

			for (auto& Write : Writes)
			{
				if(Write.Binding == -1)
					continue;

				FinalWrites.emplace_back(
					Set,
					Write.Binding,
					0,
					Write.Count,
					Write.Type,
					&Write.ImageInfo,
					&Write.BufferInfo);
			}

			Device.GetDevice().updateDescriptorSets(
				static_cast<uint32_t>(FinalWrites.size()),
				FinalWrites.data(),
				0,
				nullptr);
		}

		CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
			.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				CurrentLayout->GetPipelineLayout(),
				0,
				{ Set },
				{});
	}

	WriteMap.clear();
	HandleMap.clear();
}

void CVulkanRenderSystemContext::Draw(const uint32_t& InVertexCount,
	const uint32_t& InInstanceCount,
	const uint32_t& InFirstVertex, const uint32_t& InFirstInstance)
{
	BindDescriptorSets();

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.draw(InVertexCount, InInstanceCount, InFirstVertex, InFirstInstance);
}

void CVulkanRenderSystemContext::DrawIndexed(const uint32_t& InIndexCount,
	const uint32_t& InInstanceCount, const uint32_t& InFirstIndex,
	const int32_t& InVertexOffset, const uint32_t& InFirstInstance)
{
	BindDescriptorSets();

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.drawIndexed(InIndexCount, InInstanceCount, InFirstIndex, InVertexOffset,
			InFirstInstance);
}