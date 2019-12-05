#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"

CVulkanTexture::CVulkanTexture(CVulkanDevice* InDevice, const STextureInfo& InInfos)
	: ITexture(InInfos), CVulkanDeviceResource(InDevice), Infos(InInfos)
{
	vk::ImageCreateInfo CreateInfo(
		vk::ImageCreateFlags(),
		VulkanUtil::TextureTypeToVkImageType(InInfos.Type),
		VulkanUtil::FormatToVkFormat(InInfos.Format),
		vk::Extent3D(InInfos.Width, InInfos.Height, InInfos.Depth),
		InInfos.MipLevels,
		InInfos.ArrayLayers,
		vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eOptimal,
		VulkanUtil::TextureUsageFlagsToVkImageUsageFlags(InInfos.UsageFlags));

	VmaAllocationCreateInfo AllocInfo = {};
	AllocInfo.usage = VulkanUtil::TextureMemoryUsageToVmaMemoryUsage(InInfos.MemoryUsage);

	if (vmaCreateImage(Device->GetAllocator(),
		reinterpret_cast<VkImageCreateInfo*>(&CreateInfo),
		&AllocInfo,
		reinterpret_cast<VkImage*>(&Image),
		&Allocation,
		nullptr) != VK_SUCCESS)
		LOG(ELogSeverity::Fatal, "Failed to create Vulkan image")
}

CVulkanTexture::~CVulkanTexture()
{
	vmaDestroyImage(Device->GetAllocator(),
		Image,
		Allocation);
}

void CVulkanTexture::TransitionImageLayout(vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout)
{
	/** Allocate temporary command buffer from memory pool */
	vk::CommandBufferAllocateInfo AllocateInfo(
		g_VulkanRenderSystem->GetMemoryPool()->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	vk::UniqueCommandBuffer CommandBuffer =
		std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo).value.front());

	CommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

	vk::AccessFlags SrcAccessMask;
	vk::AccessFlags DstAccessMask;
	vk::PipelineStageFlags SrcStage;
	vk::PipelineStageFlags DstStage;

	if (InOldLayout == vk::ImageLayout::eUndefined &&
		InNewLayout == vk::ImageLayout::eTransferDstOptimal) 
	{
		DstAccessMask = vk::AccessFlagBits::eTransferWrite;

		SrcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		DstStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (InOldLayout == vk::ImageLayout::eTransferDstOptimal 
		&& InNewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) 
	{
		SrcAccessMask = vk::AccessFlagBits::eTransferWrite;
		DstAccessMask = vk::AccessFlagBits::eShaderRead;

		SrcStage = vk::PipelineStageFlagBits::eTransfer;
		DstStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		LOG(ELogSeverity::Fatal, "Unsupported transition layout")
	}

	CommandBuffer->pipelineBarrier(
		SrcStage,
		DstStage,
		vk::DependencyFlags(),
		{},
		{},
		{ vk::ImageMemoryBarrier(
			SrcAccessMask,
			DstAccessMask,
			InOldLayout,
			InNewLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			Image,
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
				0, 1, 0, 1)) });
	CommandBuffer->end();

	/** Submit to graphics queue */

	vk::SubmitInfo SubmitInfo(
		0,
		nullptr,
		nullptr,
		1,
		&*CommandBuffer,
		0,
		nullptr);

	Device->GetGraphicsQueue()->GetQueue().submit(SubmitInfo, vk::Fence());
	Device->GetGraphicsQueue()->GetQueue().waitIdle();
}