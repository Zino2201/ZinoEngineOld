#include "Render/VulkanRenderSystem/VulkanRenderSystemContext.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanQueue.h"
#include "Render/VulkanRenderSystem/VulkanCommandBuffer.h"
#include "Render/VulkanRenderSystem/VulkanSwapChain.h"
#include "Render/VulkanRenderSystem/VulkanSurface.h"
#include "Render/VulkanRenderSystem/VulkanTexture.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanPipeline.h"
#include "Render/VulkanRenderSystem/VulkanBuffer.h"
#include "Render/VulkanRenderSystem/VulkanSampler.h"

CVulkanCommandBufferManager::CVulkanCommandBufferManager(CVulkanDevice* InDevice)
	: Device(InDevice)
{
	CommandPool = Device->GetDevice().createCommandPoolUnique(
		vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlagBits::eTransient |
				vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		Device->GetGraphicsQueue()->GetFamilyIndex())).value;
	if(!CommandPool)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create command pool");

	/**
	 * Allocate 2 command buffers
	 */
	MemoryCmdBuffer = std::make_unique<CVulkanCommandBuffer>(Device, *CommandPool, false);
	if(!MemoryCmdBuffer)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to allocate memory command buffer");

	GraphicsCmdBuffer = std::make_unique<CVulkanCommandBuffer>(Device, *CommandPool, false);
	if (!GraphicsCmdBuffer)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to allocate graphics command buffer");
}

CVulkanCommandBufferManager::~CVulkanCommandBufferManager() {}

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
	Device->GetDeferredDestructionMgr().DestroyResources();

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
			Device->GetGraphicsQueue()->Submit(InCommandBuffer, { InSemaphore });
		}
		else
		{
			Device->GetGraphicsQueue()->Submit(InCommandBuffer, {});
		}
	}

	/** Free staging buffers */

	/** Reset command buffers */
}

/** RENDER SYSTEM CONTEXT */

PFN_vkCmdBeginDebugUtilsLabelEXT BeginMarker;
PFN_vkCmdEndDebugUtilsLabelEXT EndMarker;

CVulkanRenderSystemContext::CVulkanRenderSystemContext(CVulkanDevice* InDevice)
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
	vk::RenderPass RenderPass = Device->GetRenderPassFramebufferMgr().GetRenderPass(InRenderPass);
	vk::Framebuffer Framebuffer = Device->GetRenderPassFramebufferMgr().GetFramebuffer(InFramebuffer,
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
			vk::Extent2D(InFramebuffer.ColorRTs[0]->GetWidth(),
				InFramebuffer.ColorRTs[0]->GetHeight())),
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
		Surface->Present(Device->GetPresentQueue());
	}

	CurrentSurface = nullptr;
}

void CVulkanRenderSystemContext::BindGraphicsPipeline(
	CRSGraphicsPipeline* InGraphicsPipeline)
{
	CVulkanGraphicsPipeline* GraphicsPipeline =
		static_cast<CVulkanGraphicsPipeline*>(InGraphicsPipeline);

	if(CurrentLayout != GraphicsPipeline->GetPipelineLayout())
	{
		CurrentLayout = GraphicsPipeline->GetPipelineLayout();
	}

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.bindPipeline(vk::PipelineBindPoint::eGraphics, GraphicsPipeline->GetPipeline());
}

void CVulkanRenderSystemContext::BindGraphicsPipeline(const SRSGraphicsPipeline& InGraphicsPipeline)
{
	/**
	 * Get or create a pipeline
	 */
	CVulkanGraphicsPipeline* Pipeline = Device->GetPipelineManager().GetOrCreateGraphicsPipeline(
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
	must(InBuffer);
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
	must(InTexture);
	if(!InTexture)
		return;

	CVulkanTexture* Texture = static_cast<CVulkanTexture*>(InTexture);

	vk::ImageLayout ImgLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

	if(HAS_FLAG(Texture->GetTextureUsage(), ERSTextureUsage::DepthStencil))
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
	must(InSampler);
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
	must(InBinding < GMaxBindingsPerSet);

	WriteMap[InSet][InBinding] = std::move(InWrite);
	HandleMap[InSet][InBinding] = InHandle;
}

void CVulkanRenderSystemContext::BindDescriptorSets()
{
	must(CurrentLayout);

	for(const auto& [Set, Writes] : WriteMap)
	{
		const auto& Handles = HandleMap[Set];

		auto [Set, bMustUpdate] = CurrentLayout->GetSetManager().GetSet(Set,
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

			Device->GetDevice().updateDescriptorSets(
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

vk::AttachmentLoadOp VulkanUtil::RenderPass::AttachmentLoadOpToVkAttachmentLoadOp(
	const ERSRenderPassAttachmentLoadOp& InOp)
{
	switch(InOp)
	{
	default:
	case ERSRenderPassAttachmentLoadOp::DontCare:
		return vk::AttachmentLoadOp::eDontCare;
	case ERSRenderPassAttachmentLoadOp::Clear:
		return vk::AttachmentLoadOp::eClear;
	case ERSRenderPassAttachmentLoadOp::Load:
		return vk::AttachmentLoadOp::eLoad;
	}
}

vk::AttachmentStoreOp VulkanUtil::RenderPass::AttachmentStoreOpToVkAttachmentStoreOp(
	const ERSRenderPassAttachmentStoreOp& InOp)
{
	switch (InOp)
	{
	default:
	case ERSRenderPassAttachmentStoreOp::DontCare:
		return vk::AttachmentStoreOp::eDontCare;
	case ERSRenderPassAttachmentStoreOp::Store:
		return vk::AttachmentStoreOp::eStore;
	}
}

vk::ImageLayout VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
	const ERSRenderPassAttachmentLayout& InLayout)
{
	switch(InLayout)
	{
	default:
	case ERSRenderPassAttachmentLayout::Undefined:
		return vk::ImageLayout::eUndefined;
	case ERSRenderPassAttachmentLayout::ColorAttachment:
		return vk::ImageLayout::eColorAttachmentOptimal;
	case ERSRenderPassAttachmentLayout::ShaderReadOnlyOptimal:
		return vk::ImageLayout::eShaderReadOnlyOptimal;
	case ERSRenderPassAttachmentLayout::DepthStencilAttachment:
		return vk::ImageLayout::eDepthStencilAttachmentOptimal;
	case ERSRenderPassAttachmentLayout::DepthStencilReadOnlyOptimal:
		return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
	case ERSRenderPassAttachmentLayout::Present:
		return vk::ImageLayout::ePresentSrcKHR;
	}
}