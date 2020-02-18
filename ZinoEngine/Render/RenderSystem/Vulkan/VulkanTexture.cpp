#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffer.h"

CVulkanTexture::CVulkanTexture(CVulkanDevice* InDevice, const SRenderSystemTextureInfo& InInfos)
	: CRenderSystemTexture(InInfos), CVulkanDeviceResource(InDevice), Infos(InInfos)
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
	AllocInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
	AllocInfo.pUserData = const_cast<char*>(InInfos.DebugName.c_str());

	if (vmaCreateImage(Device->GetAllocator(),
		reinterpret_cast<VkImageCreateInfo*>(&CreateInfo),
		&AllocInfo,
		reinterpret_cast<VkImage*>(&Image),
		&Allocation,
		nullptr) != VK_SUCCESS)
		LOG(ELogSeverity::Fatal, "Failed to create Vulkan image")
}

void CVulkanTexture::Destroy()
{
	LOG(ELogSeverity::Debug, "Destroyed texture(image) %s",
		Infos.DebugName.c_str())

	vmaDestroyImage(Device->GetAllocator(),
		Image,
		Allocation);
}

void CVulkanTexture::Copy(CRenderSystemBuffer* InSrc)
{
	CVulkanBuffer* Src = static_cast<CVulkanBuffer*>(InSrc);

	/** Transition image layout before copying */
	TransitionImageLayout(vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	/** Allocate temporary command buffer from memory pool */
	vk::CommandBufferAllocateInfo AllocateInfo(
		g_VulkanRenderSystem->GetMemoryPool()->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	vk::UniqueCommandBuffer CommandBuffer =
		std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo).value.front());

	CommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));;
	CommandBuffer->copyBufferToImage(Src->GetBuffer(),
		Image,
		vk::ImageLayout::eTransferDstOptimal,
		{ vk::BufferImageCopy(0, 0, 0,
			vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
				0,
				0,
				1),
			vk::Offset3D(),
			vk::Extent3D(Infos.Width, Infos.Height, Infos.Depth)) });
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

	/** Generate mipmaps */
	GenerateMipmaps();
}

void CVulkanTexture::GenerateMipmaps()
{
	/** Allocate temporary command buffer from memory pool */
	vk::CommandBufferAllocateInfo AllocateInfo(
		g_VulkanRenderSystem->GetMemoryPool()->GetCommandPool(),
		vk::CommandBufferLevel::ePrimary,
		1);

	vk::UniqueCommandBuffer CommandBuffer =
		std::move(Device->GetDevice().allocateCommandBuffersUnique(AllocateInfo).value.front());

	CommandBuffer->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	
	uint32_t MipWidth = Infos.Width;
	uint32_t MipHeight = Infos.Height;

	for(uint32_t i = 1; i < Infos.MipLevels; ++i)
	{
		CommandBuffer->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eTransfer,
			vk::DependencyFlags(),
			{},
			{},
			{ 
				vk::ImageMemoryBarrier(
					vk::AccessFlagBits::eTransferWrite,
					vk::AccessFlagBits::eTransferRead,
					vk::ImageLayout::eTransferDstOptimal,
					vk::ImageLayout::eTransferSrcOptimal,
					VK_QUEUE_FAMILY_IGNORED,
					VK_QUEUE_FAMILY_IGNORED,
					Image,
					vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
						i - 1, 1, 0, 1))
			});

		CommandBuffer->blitImage(
			Image, vk::ImageLayout::eTransferSrcOptimal,
			Image, vk::ImageLayout::eTransferDstOptimal,
			{ 
				vk::ImageBlit(
					vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
						i - 1, 0, 1),
					{ vk::Offset3D(), vk::Offset3D(MipWidth, MipHeight, 1) },
					vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
						i, 0, 1),
					{ vk::Offset3D(), vk::Offset3D(MipWidth > 1 ? MipWidth / 2 : 1, 
						MipHeight > 1 ? MipHeight / 2 : 1, 1) }) 
			}, 
			vk::Filter::eLinear);

		CommandBuffer->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eFragmentShader,
			vk::DependencyFlags(),
			{},
			{},
			{
				vk::ImageMemoryBarrier(
					vk::AccessFlagBits::eTransferRead,
					vk::AccessFlagBits::eShaderRead,
					vk::ImageLayout::eTransferSrcOptimal,
					vk::ImageLayout::eShaderReadOnlyOptimal,
					VK_QUEUE_FAMILY_IGNORED,
					VK_QUEUE_FAMILY_IGNORED,
					Image,
					vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
						i - 1, 1, 0, 1))
			});

		if (MipWidth > 1) MipWidth /= 2;
		if (MipHeight > 1) MipHeight /= 2;
	}

	CommandBuffer->pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eFragmentShader,
		vk::DependencyFlags(),
		{},
		{},
			{
				vk::ImageMemoryBarrier(
					vk::AccessFlagBits::eTransferWrite,
					vk::AccessFlagBits::eShaderRead,
					vk::ImageLayout::eTransferDstOptimal,
					vk::ImageLayout::eShaderReadOnlyOptimal,
					VK_QUEUE_FAMILY_IGNORED,
					VK_QUEUE_FAMILY_IGNORED,
					Image,
					vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
						Infos.MipLevels - 1, 1, 0, 1))
			});

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
				0, Infos.MipLevels, 0, 1)) });
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