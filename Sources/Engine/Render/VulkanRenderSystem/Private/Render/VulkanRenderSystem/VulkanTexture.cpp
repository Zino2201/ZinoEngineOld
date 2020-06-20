#include "Render/VulkanRenderSystem/VulkanTexture.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"
#include "Render/VulkanRenderSystem/VulkanCommandBuffer.h"

vk::ImageAspectFlagBits VulkanUtil::GetImageAspectFromFormat(const EFormat& InFormat)
{
	switch(InFormat)
	{
	default:
		return vk::ImageAspectFlagBits::eColor;
	case EFormat::D24UnormS8Uint:
	case EFormat::D32Sfloat:
	case EFormat::D32SfloatS8Uint:
		return vk::ImageAspectFlagBits::eDepth;
	}
}

CVulkanTexture::CVulkanTexture(CVulkanDevice* InDevice, 
	const ERSTextureType& InTextureType,
	const ERSTextureUsage& InTextureUsage,
	const ERSMemoryUsage& InMemoryUsage,
	const EFormat& InFormat, 
	uint32_t InWidth,
	uint32_t InHeight,
	uint32_t InDepth,
	uint32_t InArraySize,
	uint32_t InMipLevels,
	const ESampleCount& InSampleCount,
	const SRSResourceCreateInfo& InCreateInfo)
	: CVulkanDeviceResource(InDevice),
	CRSTexture(InTextureType, InTextureUsage, InMemoryUsage,
		InFormat, InWidth, InHeight, InDepth, InArraySize, InMipLevels, InSampleCount, 
		InCreateInfo),
	bShouldDestroyImage(true)
{
	{
		vk::ImageUsageFlags UsageFlags =
			vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;

		if (HAS_FLAG(InTextureUsage, ERSTextureUsage::Sampled))
		{
			UsageFlags |= vk::ImageUsageFlagBits::eSampled;
		}

		if (HAS_FLAG(InTextureUsage, ERSTextureUsage::RenderTarget))
		{
			UsageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
			UsageFlags |= vk::ImageUsageFlagBits::eInputAttachment;
		}

		if (HAS_FLAG(InTextureUsage, ERSTextureUsage::DepthStencil))
		{
			UsageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
			UsageFlags |= vk::ImageUsageFlagBits::eInputAttachment;
		}

		vk::ImageType ImageType;
		switch(InTextureType)
		{
		case ERSTextureType::Tex1D:
			ImageType = vk::ImageType::e1D;
			break;
		case ERSTextureType::Tex2D:
			ImageType = vk::ImageType::e2D;
			break;
		case ERSTextureType::Tex3D:
			ImageType = vk::ImageType::e3D;
			break;
		}

		vk::ImageCreateInfo CreateInfo(
			vk::ImageCreateFlags(),
			ImageType,
			VulkanUtil::FormatToVkFormat(InFormat),
			vk::Extent3D(Width, Height, InDepth),
			InMipLevels,
			InArraySize,
			VulkanUtil::SampleCountToVkSampleCount(InSampleCount),
			vk::ImageTiling::eOptimal,
			UsageFlags);

		VmaAllocationCreateInfo AllocInfo = {};
		AllocInfo.usage = VulkanUtil::BufferUsageFlagsToMemoryUsage(InMemoryUsage);
		AllocInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
		AllocInfo.pUserData = const_cast<char*>(InCreateInfo.DebugName);
		
		VkResult Res;
		Res = vmaCreateImage(Device->GetAllocator(),
			reinterpret_cast<VkImageCreateInfo*>(&CreateInfo),
			&AllocInfo,
			reinterpret_cast<VkImage*>(&Image),
			&Allocation,
			nullptr);
		if(Res != VK_SUCCESS)
		{
			const char* ErrorMsg = "Unknown error (check arguments passed)";

			if(Res == VK_ERROR_EXTENSION_NOT_PRESENT)
				ErrorMsg = "No memory type meeting all requirements has been found";

			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create Vulkan image: %s", ErrorMsg);
		}
	}

	/** Create image view */
	{
		vk::ImageViewType ImageType;
		switch (InTextureType)
		{
		case ERSTextureType::Tex1D:
			ImageType = vk::ImageViewType::e1D;
			break;
		case ERSTextureType::Tex2D:
			ImageType = vk::ImageViewType::e2D;
			break;
		case ERSTextureType::Tex3D:
			ImageType = vk::ImageViewType::e3D;
			break;
		}

		vk::ImageViewCreateInfo ViewCreateInfo(vk::ImageViewCreateFlags(),
			Image,
			ImageType,
			VulkanUtil::FormatToVkFormat(InFormat),
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				VulkanUtil::GetImageAspectFromFormat(InFormat), 0, InMipLevels, 0, 1));

		vk::Result ResView = Device->GetDevice().createImageView(
			&ViewCreateInfo,
			nullptr,
			&ImageView);
		if (!ImageView)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create image view: %s",
				vk::to_string(ResView).c_str());
	}

	/** Copy initial data */
	if(InCreateInfo.InitialData)
	{
		/** Copy pixels to staging buffer for copy */
		uint64_t BufSize = InWidth * InHeight * 4;
		
		CVulkanInternalStagingBuffer* StagingBuf = 
			Device->GetStagingBufferMgr()->CreateStagingBuffer(BufSize,
				vk::BufferUsageFlagBits::eTransferSrc);
		memcpy(StagingBuf->GetAllocationInfo().pMappedData, InCreateInfo.InitialData, BufSize);
		CopyFromBuffer(StagingBuf->GetBuffer());
		Device->GetStagingBufferMgr()->ReleaseStagingBuffer(StagingBuf);
	}
}

CVulkanTexture::CVulkanTexture(CVulkanDevice* InDevice,
	const ERSTextureType& InTextureType,
	const ERSTextureUsage& InTextureUsage,
	const ERSMemoryUsage& InMemoryUsage,
	const EFormat& InFormat,
	const uint32_t& InWidth,
	const uint32_t& InHeight,
	const uint32_t& InDepth,
	const uint32_t& InArraySize,
	const uint32_t& InMipLevels,
	const ESampleCount& InSampleCount,
	const vk::Image& InImage,
	const SRSResourceCreateInfo& InCreateInfo)
	: CVulkanDeviceResource(InDevice),
		CRSTexture(InTextureType, InTextureUsage, InMemoryUsage,
			InFormat, InWidth, InHeight, InDepth, InArraySize, InMipLevels, InSampleCount,
			InCreateInfo),
		bShouldDestroyImage(false),
	Image(InImage)
{
	vk::ImageViewType ImageType;
	switch (InTextureType)
	{
	case ERSTextureType::Tex1D:
		ImageType = vk::ImageViewType::e1D;
		break;
	case ERSTextureType::Tex2D:
		ImageType = vk::ImageViewType::e2D;
		break;
	case ERSTextureType::Tex3D:
		ImageType = vk::ImageViewType::e3D;
		break;
	}

	vk::ImageViewCreateInfo ViewCreateInfo(vk::ImageViewCreateFlags(),
		Image,
		ImageType,
		VulkanUtil::FormatToVkFormat(InFormat),
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			VulkanUtil::GetImageAspectFromFormat(InFormat), 0, InMipLevels, 0, 1));

	vk::Result ResView = Device->GetDevice().createImageView(
		&ViewCreateInfo,
		nullptr,
		&ImageView);
	if (!ImageView)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create image view: %s",
			vk::to_string(ResView).c_str());

#ifndef NDEBUG
	PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT) 
		GVulkanRenderSystem->GetInstance().getProcAddr("vkSetDebugUtilsObjectNameEXT");

	VkDebugUtilsObjectNameInfoEXT Info;
	Info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	Info.pNext = nullptr;
	Info.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
	Info.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(ImageView));
	Info.pObjectName = InCreateInfo.DebugName;

	SetDebugUtilsObjectName(static_cast<VkDevice>(Device->GetDevice()), &Info);
#endif
}

CVulkanTexture::~CVulkanTexture()
{
	if(bShouldDestroyImage)
	{
		Device->GetDeferredDestructionMgr().EnqueueImage(
			CVulkanDeferredDestructionManager::EHandleType::Image,
			Allocation,
			Image);	
	}

	Device->GetDeferredDestructionMgr().EnqueueImageView(
		CVulkanDeferredDestructionManager::EHandleType::ImageView,
		ImageView);
}

void CVulkanTexture::CopyFromBuffer(const vk::Buffer& InBuffer)
{
	/** Transition image layout before copying */
	TransitionImageLayout(vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	/** Do the actual copy */
	const vk::CommandBuffer& CmdBuffer =
		Device->GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	Device->GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

	CmdBuffer.copyBufferToImage(InBuffer,
		Image,
		vk::ImageLayout::eTransferDstOptimal,
		{ vk::BufferImageCopy(0, 0, 0,
			vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
				0,
				0,
				1),
			vk::Offset3D(),
			vk::Extent3D(Width, Height, Depth)) });

	Device->GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
	Device->WaitIdle();

	/** (Re-)generate mipmaps */
	GenerateMipmaps();
}

void CVulkanTexture::GenerateMipmaps()
{
	const vk::CommandBuffer& CmdBuffer =
		Device->GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	Device->GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

	uint32_t MipWidth = Width;
	uint32_t MipHeight = Height;
	
	for (uint32_t i = 1; i < MipLevels; ++i)
	{
		CmdBuffer.pipelineBarrier(
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

		CmdBuffer.blitImage(
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

		CmdBuffer.pipelineBarrier(
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

	CmdBuffer.pipelineBarrier(
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
					MipLevels - 1, 1, 0, 1))
		});

	Device->GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
	Device->WaitIdle();
}

void CVulkanTexture::TransitionImageLayout(const vk::ImageLayout& InOldLayout,
	const vk::ImageLayout& InNewLayout)
{
	const vk::CommandBuffer& CmdBuffer =
		Device->GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	Device->GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();
	
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
		LOG(ELogSeverity::Fatal, VulkanRS, "Unsupported transition layout");
	}

	CmdBuffer.pipelineBarrier(
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
				0, MipLevels, 0, 1)) });

	Device->GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
	Device->WaitIdle();
}

CRSTexture* CVulkanRenderSystem::CreateTexture(
	const ERSTextureType& InTextureType,
	const ERSTextureUsage& InTextureUsage,
	const ERSMemoryUsage& InMemoryUsage,
	const EFormat& InFormat,
	const uint32_t& InWidth,
	const uint32_t& InHeight,
	const uint32_t& InDepth,
	const uint32_t& InArraySize,
	const uint32_t& InMipLevels,
	const ESampleCount& InSampleCount,
	const SRSResourceCreateInfo& InInfo) const
{
	return new CVulkanTexture(Device.get(),
		InTextureType,
		InTextureUsage,
		InMemoryUsage,
		InFormat,
		InWidth,
		InHeight,
		InDepth,
		InArraySize,
		InMipLevels,
		InSampleCount,
		InInfo);
}