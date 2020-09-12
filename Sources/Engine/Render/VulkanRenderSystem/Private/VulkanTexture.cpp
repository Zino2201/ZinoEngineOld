#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanCommandBuffer.h"

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

CVulkanTexture::CVulkanTexture(CVulkanDevice& InDevice, 
	const SRSTextureCreateInfo& InCreateInfo)
	: CVulkanDeviceResource(InDevice),
	CRSTexture(InCreateInfo),
	bShouldDestroyImage(true)
{
	{
		vk::ImageUsageFlags UsageFlags =
			vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;

		if (InCreateInfo.Usage & ERSTextureUsageFlagBits::Sampled)
		{
			UsageFlags |= vk::ImageUsageFlagBits::eSampled;
		}

		if (InCreateInfo.Usage & ERSTextureUsageFlagBits::RenderTarget)
		{
			UsageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
			UsageFlags |= vk::ImageUsageFlagBits::eInputAttachment;
		}

		if (InCreateInfo.Usage & ERSTextureUsageFlagBits::DepthStencil)
		{
			UsageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
			UsageFlags |= vk::ImageUsageFlagBits::eInputAttachment;
		}

		vk::ImageType ImageType;
		switch(InCreateInfo.Type)
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
			VulkanUtil::FormatToVkFormat(InCreateInfo.Format),
			vk::Extent3D(InCreateInfo.Width, InCreateInfo.Height, InCreateInfo.Depth),
			InCreateInfo.MipLevels,
			InCreateInfo.ArraySize,
			VulkanUtil::SampleCountToVkSampleCount(InCreateInfo.SampleCount),
			vk::ImageTiling::eOptimal,
			UsageFlags);

		VmaAllocationCreateInfo AllocInfo = {};
		AllocInfo.usage = VulkanUtil::BufferUsageFlagsToMemoryUsage(InCreateInfo.MemoryUsage);
		AllocInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
		
		VkResult Res;
		Res = vmaCreateImage(Device.GetAllocator(),
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

			ZE::Logger::Fatal("Failed to create Vulkan image: {}", ErrorMsg);
		}
	}

	/** Create image view */
	{
		vk::ImageViewType ImageType;
		switch (InCreateInfo.Type)
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
			VulkanUtil::FormatToVkFormat(CreateInfo.Format),
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				VulkanUtil::GetImageAspectFromFormat(CreateInfo.Format), 0, CreateInfo.MipLevels, 0, 1));

		vk::Result ResView = Device.GetDevice().createImageView(
			&ViewCreateInfo,
			nullptr,
			&ImageView);
		if (!ImageView)
			ZE::Logger::Fatal("Failed to create image view: {}",
				vk::to_string(ResView).c_str());
	}
}

CVulkanTexture::CVulkanTexture(CVulkanDevice& InDevice,
	const SRSTextureCreateInfo& InCreateInfo,
	const vk::Image& InImage)
	: CVulkanDeviceResource(InDevice),
		CRSTexture(InCreateInfo),
		bShouldDestroyImage(false),
	Image(InImage)
{
	vk::ImageViewType ImageType;
	switch (InCreateInfo.Type)
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
		VulkanUtil::FormatToVkFormat(CreateInfo.Format),
		vk::ComponentMapping(),
		vk::ImageSubresourceRange(
			VulkanUtil::GetImageAspectFromFormat(CreateInfo.Format), 0, CreateInfo.MipLevels, 0, 1));

	vk::Result ResView = Device.GetDevice().createImageView(
		&ViewCreateInfo,
		nullptr,
		&ImageView);
	if (!ImageView)
		ZE::Logger::Fatal("Failed to create image view: {}",
			vk::to_string(ResView).c_str());
}

CVulkanTexture::~CVulkanTexture()
{
	if(bShouldDestroyImage)
	{
		Device.GetDeferredDestructionMgr().EnqueueImage(
			CVulkanDeferredDestructionManager::EHandleType::Image,
			Allocation,
			Image);	
	}

	Device.GetDeferredDestructionMgr().EnqueueImageView(
		CVulkanDeferredDestructionManager::EHandleType::ImageView,
		ImageView);
}

void CVulkanTexture::SetName(const char* InName)
{
	CRSResource::SetName(InName);

	/** Don't set anything if we don't own the texture */
	if(bShouldDestroyImage)
		vmaSetAllocationUserData(Device.GetAllocator(),
			Allocation, reinterpret_cast<void*>(const_cast<char*>(InName)));

#if ZE_ENABLE_VULKAN_DEBUG_FEATURES
	PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)
		GVulkanRenderSystem->GetInstance().getProcAddr("vkSetDebugUtilsObjectNameEXT");

	VkDebugUtilsObjectNameInfoEXT Info;
	Info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	Info.pNext = nullptr;
	Info.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
	Info.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(ImageView));
	Info.pObjectName = InName;

	SetDebugUtilsObjectName(static_cast<VkDevice>(Device.GetDevice()), &Info);
#endif
}

void CVulkanTexture::Copy(const void* Src)
{
	/** Copy pixels to staging buffer for copy */
	uint64_t BufSize = CreateInfo.Width * CreateInfo.Height * 4;

	CVulkanInternalStagingBuffer* StagingBuf =
		Device.GetStagingBufferMgr().CreateStagingBuffer(BufSize,
			vk::BufferUsageFlagBits::eTransferSrc);
	memcpy(StagingBuf->GetAllocationInfo().pMappedData, Src, BufSize);
	CopyFromBuffer(StagingBuf->GetBuffer());
	Device.GetStagingBufferMgr().ReleaseStagingBuffer(StagingBuf);
}

void CVulkanTexture::CopyFromBuffer(const vk::Buffer& InBuffer)
{
	/** Transition image layout before copying */
	TransitionImageLayout(vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	/** Do the actual copy */
	const vk::CommandBuffer& CmdBuffer =
		Device.GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	Device.GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

	CmdBuffer.copyBufferToImage(InBuffer,
		Image,
		vk::ImageLayout::eTransferDstOptimal,
		{ vk::BufferImageCopy(0, 0, 0,
			vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
				0,
				0,
				1),
			vk::Offset3D(),
			vk::Extent3D(CreateInfo.Width, CreateInfo.Height, CreateInfo.Depth)) });

	Device.GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
	Device.WaitIdle();

	/** (Re-)generate mipmaps */
	GenerateMipmaps();
}

void CVulkanTexture::GenerateMipmaps()
{
	const vk::CommandBuffer& CmdBuffer =
		Device.GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	Device.GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();

	uint32_t MipWidth = CreateInfo.Width;
	uint32_t MipHeight = CreateInfo.Height;
	
	for (uint32_t i = 1; i < CreateInfo.MipLevels; ++i)
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
					CreateInfo.MipLevels - 1, 1, 0, 1))
		});

	Device.GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
	Device.WaitIdle();
}

void CVulkanTexture::TransitionImageLayout(const vk::ImageLayout& InOldLayout,
	const vk::ImageLayout& InNewLayout)
{
	const vk::CommandBuffer& CmdBuffer =
		Device.GetContext()->GetCmdBufferMgr().GetMemoryCmdBuffer()->GetCommandBuffer();

	Device.GetContext()->GetCmdBufferMgr().BeginMemoryCmdBuffer();
	
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
		ZE::Logger::Fatal("Unsupported transition layout");
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
				0, CreateInfo.MipLevels, 0, 1)) });

	Device.GetContext()->GetCmdBufferMgr().SubmitMemoryCmdBuffer();
	Device.WaitIdle();
}

TOwnerPtr<CRSTexture> CVulkanRenderSystem::CreateTexture(
	const SRSTextureCreateInfo& InCreateInfo) const
{
	return new CVulkanTexture(*Device.get(),
		InCreateInfo);
}