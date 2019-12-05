#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"

CVulkanCommandBuffer::CVulkanCommandBuffer(CVulkanDevice* InDevice,
	CVulkanCommandPool* InCommandPool)
	: CVulkanDeviceResource(InDevice), CommandPool(InCommandPool)
{
	vk::CommandBufferAllocateInfo AllocateInfo(
		CommandPool->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	CommandBuffer = std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo)
		.value.front());
}

CVulkanCommandBuffer::~CVulkanCommandBuffer() {}

void CVulkanCommandBuffer::AddWaitSemaphore(const vk::PipelineStageFlagBits& InStageFlag,
	const vk::Semaphore& InSemaphore)
{
	WaitFlags |= InStageFlag;
	WaitSemaphores.emplace_back(InSemaphore);
}

void CVulkanCommandBuffer::SubmitSemaphores()
{
	WaitFlags = vk::PipelineStageFlags();
	WaitSemaphores.clear();
}

void CVulkanCommandBuffer::Begin()
{
	vk::CommandBufferBeginInfo BeginInfo(
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	CommandBuffer->begin(BeginInfo);
}

void CVulkanCommandBuffer::BeginRenderPass(const vk::RenderPass& InRenderPass,
	const vk::Framebuffer& InFramebuffer, const std::array<float, 4>& InClearColor)
{
	std::array<vk::ClearValue, 2> ClearValues =
	{
		vk::ClearColorValue(InClearColor),
		vk::ClearDepthStencilValue(1.f, 0)
	};

	vk::RenderPassBeginInfo BeginInfo(
		InRenderPass,
		InFramebuffer,
		vk::Rect2D(vk::Offset2D(), g_VulkanRenderSystem->GetSwapChain()->GetExtent()),
		static_cast<uint32_t>(ClearValues.size()),
		ClearValues.data());

	CommandBuffer->beginRenderPass(BeginInfo, vk::SubpassContents::eInline);
}

void CVulkanCommandBuffer::EndRenderPass()
{
	CommandBuffer->endRenderPass();
}

void CVulkanCommandBuffer::End()
{
	CommandBuffer->end();
}