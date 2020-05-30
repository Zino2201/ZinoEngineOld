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

CVulkanRenderPassManager::CVulkanRenderPassManager(CVulkanDevice* InDevice)
	: Device(InDevice) {}

CVulkanRenderPassManager::~CVulkanRenderPassManager() 
{
	for(const auto& [Framebuffer, Handle] : Framebuffers)
	{
		Device->GetDevice().destroyFramebuffer(Handle);
	}

	for (const auto& [RenderPass, Handle] : RenderPasses)
	{
		Device->GetDevice().destroyRenderPass(Handle);
	}
}

vk::RenderPass CVulkanRenderPassManager::GetRenderPass(const SRSRenderPass& InRenderPass)
{
	auto& FoundRenderPass = RenderPasses.find(InRenderPass);

	if(FoundRenderPass != RenderPasses.end())
	{
		return FoundRenderPass->second;
	}
	else
	{
		/** Create render pass */
		std::vector<vk::AttachmentDescription> Attachments;
		Attachments.reserve(InRenderPass.ColorAttachments.size());

		for(const auto& ColorAttachment : InRenderPass.ColorAttachments)
		{
			Attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),
				VulkanUtil::FormatToVkFormat(ColorAttachment.Format),
				VulkanUtil::SampleCountToVkSampleCount(ColorAttachment.SampleCount),
				VulkanUtil::RenderPass::AttachmentLoadOpToVkAttachmentLoadOp(ColorAttachment.Load),
				VulkanUtil::RenderPass::AttachmentStoreOpToVkAttachmentStoreOp(ColorAttachment.Store),
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(ColorAttachment.InitialLayout),
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(ColorAttachment.FinalLayout));
		}

		for (const auto& DepthAttachment : InRenderPass.DepthAttachments)
		{
			Attachments.emplace_back(
				vk::AttachmentDescriptionFlags(),
				VulkanUtil::FormatToVkFormat(DepthAttachment.Format),
				VulkanUtil::SampleCountToVkSampleCount(DepthAttachment.SampleCount),
				VulkanUtil::RenderPass::AttachmentLoadOpToVkAttachmentLoadOp(DepthAttachment.Load),
				VulkanUtil::RenderPass::AttachmentStoreOpToVkAttachmentStoreOp(DepthAttachment.Store),
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(DepthAttachment.InitialLayout),
				VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(DepthAttachment.FinalLayout));
		}

		std::vector<vk::SubpassDescription> Subpasses;
		Subpasses.reserve(InRenderPass.Subpasses.size());

		/**
		 * We need to keep attachment ref alive for createRenderPass
		 */
		std::vector<vk::AttachmentReference> ColorAttachments;
		std::vector<vk::AttachmentReference> DepthAttachments;
		std::vector<vk::AttachmentReference> InputAttachments;
		size_t FirstIdx = 0;

		for(const auto& Subpass : InRenderPass.Subpasses)
		{
			for(const auto& ColorAttachment : Subpass.ColorAttachmentRefs)
			{
				ColorAttachments.emplace_back(
					ColorAttachment.Index,
					VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
						ColorAttachment.Layout));
			}

			for (const auto& DepthAttachment : Subpass.DepthAttachmentRefs)
			{
				DepthAttachments.emplace_back(
					DepthAttachment.Index,
					VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
						DepthAttachment.Layout));
			}

			for (const auto& InputAttachment : Subpass.InputAttachmentRefs)
			{
				InputAttachments.emplace_back(
					InputAttachment.Index,
					VulkanUtil::RenderPass::AttachmentLayoutToVkImageLayout(
						InputAttachment.Layout));
			}

			Subpasses.emplace_back(
				vk::SubpassDescriptionFlags(),
				vk::PipelineBindPoint::eGraphics,
				static_cast<uint32_t>(InputAttachments.size()),
				InputAttachments.data(),
				static_cast<uint32_t>(Subpass.ColorAttachmentRefs.size()),
				ColorAttachments.empty() ? nullptr : &ColorAttachments[FirstIdx],
				nullptr,
				DepthAttachments.empty() ? nullptr : &DepthAttachments[FirstIdx]);

			FirstIdx += Subpass.ColorAttachmentRefs.size();
		}

		vk::RenderPassCreateInfo CreateInfo = vk::RenderPassCreateInfo(
			vk::RenderPassCreateFlags(),
			static_cast<uint32_t>(Attachments.size()),
			Attachments.data(),
			static_cast<uint32_t>(Subpasses.size()),
			Subpasses.data());
		
		vk::RenderPass RenderPass = Device->GetDevice().createRenderPass(
			CreateInfo).value;
		if(!RenderPass)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create render pass");

		LOG(ELogSeverity::Debug, VulkanRS, "new vk::RenderPass");

		RenderPasses.insert(std::make_pair(InRenderPass, RenderPass));

		return RenderPass;
	}
}

vk::Framebuffer CVulkanRenderPassManager::GetFramebuffer(const SRSFramebuffer& InFramebuffer,
	const vk::RenderPass& InRenderPass)
{
	auto& FoundFramebuffer = Framebuffers.find(InFramebuffer);

	if(FoundFramebuffer != Framebuffers.end())
	{
		return FoundFramebuffer->second;
	}
	else
	{
		uint32_t Width = 0;
		uint32_t Height = 0;

		std::vector<vk::ImageView> Attachments;
		for(int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
		{
			CRSTexture* RT = InFramebuffer.ColorRTs[i];
			if(!RT)
				continue;

			CVulkanTexture* VkRt =
				static_cast<CVulkanTexture*>(RT);

			Width = std::max(Width, RT->GetWidth());
			Height = std::max(Height, RT->GetHeight());

			Attachments.push_back(VkRt->GetImageView());
		}

		for (int i = 0; i < GMaxRenderTargetPerFramebuffer; ++i)
		{
			CRSTexture* RT = InFramebuffer.DepthRTs[i];
			if (!RT)
				continue;

			CVulkanTexture* VkRt =
				static_cast<CVulkanTexture*>(RT);

			Attachments.push_back(VkRt->GetImageView());
		}

		vk::FramebufferCreateInfo CreateInfo = vk::FramebufferCreateInfo(
			vk::FramebufferCreateFlags(),
			InRenderPass,
			static_cast<uint32_t>(Attachments.size()),
			Attachments.data(),
			Width,
			Height,
			1);

		vk::Framebuffer Framebuffer = Device->GetDevice().createFramebuffer(
			CreateInfo).value;
		if (!Framebuffer)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create framebuffer");

		LOG(ELogSeverity::Debug, VulkanRS, "new vk::Framebuffer");

		Framebuffers.insert(std::make_pair(InFramebuffer, Framebuffer));

		return Framebuffer;
	}
}

/** RENDER SYSTEM CONTEXT */

PFN_vkCmdBeginDebugUtilsLabelEXT BeginMarker;
PFN_vkCmdEndDebugUtilsLabelEXT EndMarker;

CVulkanRenderSystemContext::CVulkanRenderSystemContext(CVulkanDevice* InDevice)
	: Device(InDevice), CmdBufferMgr(InDevice), RenderPassMgr(InDevice), 
	CurrentLayout(nullptr), ColorAttachmentsCount(0)
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
	vk::RenderPass RenderPass = RenderPassMgr.GetRenderPass(InRenderPass);
	vk::Framebuffer Framebuffer = RenderPassMgr.GetFramebuffer(InFramebuffer,
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

void CVulkanRenderSystemContext::BeginSurface(CRSSurface* InSurface)
{
	must(InSurface);

	CVulkanSurface* Surface = static_cast<CVulkanSurface*>(InSurface);

	Surface->GetSwapChain()->AcquireImage();
}

void CVulkanRenderSystemContext::PresentSurface(CRSSurface* InSurface)
{
	must(InSurface);

	CVulkanSurface* Surface = static_cast<CVulkanSurface*>(InSurface);

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
		Surface->GetSwapChain()->Present(Device->GetPresentQueue());
	}
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
	
	AddWrite(InSet, WriteSet, std::hash<CRSBuffer*>()(InBuffer));
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

	AddWrite(InSet, WriteSet, std::hash<CRSTexture*>()(InTexture));
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

	AddWrite(InSet, WriteSet, std::hash<CRSSampler*>()(InSampler));
}

void CVulkanRenderSystemContext::AddWrite(const uint32_t& InSet, const SDescriptorSetWrite& InWrite,
	const uint64_t& InResourceHash)
{
	{
		auto& Result = WriteSetMap.find(InSet);

		if (Result == WriteSetMap.end())
			WriteSetMap.insert({ InSet, {} });

		WriteSetMap[InSet].push_back(InWrite);
	}

	{
		auto& Result = ResourceSetHashMap.find(InSet);

		if (Result == ResourceSetHashMap.end())
			ResourceSetHashMap.insert({ InSet, {} });

		ResourceSetHashMap[InSet].push_back(InResourceHash);
	}
}

void CVulkanRenderSystemContext::FlushWrites()
{
	must(CurrentLayout);

	/**
	 * Find descriptor sets compatible with the writes
	 */

	/**
	 * Sets to allocate
	 */
	std::vector<uint32_t> SetsToAllocate;
	SetsToAllocate.reserve(ResourceSetHashMap.size());

	for(const auto& [Set, Hashes] : ResourceSetHashMap)
	{
		SDescriptorSetEntry Entry;
		Entry.Hashes = Hashes;

		auto& Result = DescriptorSetMap.find(Entry);
		if(Result != DescriptorSetMap.end())
		{
			CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
				.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				CurrentLayout->GetPipelineLayout(),
					0,
					{ Result->second },
					{});
		}
		else
		{
			SetsToAllocate.push_back(Set);
		}
	}

	for(const auto& SetToAllocate : SetsToAllocate)
	{
		LOG(ELogSeverity::Debug, VulkanRS, "Allocated a new set !");

		vk::DescriptorPool Pool;

		vk::DescriptorSet Set = CurrentLayout->GetCacheMgr()
			->AllocateDescriptorSet(SetToAllocate, Pool);

		/** Update descriptor set */
		auto& Writes = WriteSetMap[SetToAllocate];

		std::vector<vk::WriteDescriptorSet> FinalWrites;
		FinalWrites.reserve(Writes.size());

		for(auto& Write : Writes)
		{
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

		SDescriptorSetEntry Entry;
		Entry.Hashes = ResourceSetHashMap[SetToAllocate];
		Entry.Pool = Pool;
		DescriptorSetMap.insert(std::make_pair(Entry, Set));

		CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
			.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				CurrentLayout->GetPipelineLayout(),
				0,
				{ Set },
				{});
	}

	WriteSetMap.clear();
	ResourceSetHashMap.clear();
}

void CVulkanRenderSystemContext::Draw(const uint32_t& InVertexCount,
	const uint32_t& InInstanceCount,
	const uint32_t& InFirstVertex, const uint32_t& InFirstInstance)
{
	FlushWrites();

	CmdBufferMgr.GetGraphicsCmdBuffer()->GetCommandBuffer()
		.draw(InVertexCount, InInstanceCount, InFirstVertex, InFirstInstance);
}

void CVulkanRenderSystemContext::DrawIndexed(const uint32_t& InIndexCount,
	const uint32_t& InInstanceCount, const uint32_t& InFirstIndex,
	const int32_t& InVertexOffset, const uint32_t& InFirstInstance)
{
	FlushWrites();

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