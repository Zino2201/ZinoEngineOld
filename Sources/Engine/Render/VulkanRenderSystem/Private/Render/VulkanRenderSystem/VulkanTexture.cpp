#include "Render/VulkanRenderSystem/VulkanTexture.h"
#include "Render/VulkanRenderSystem/VulkanDevice.h"
#include "Render/VulkanRenderSystem/VulkanRenderSystem.h"

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

		ImageView = Device->GetDevice().createImageView(
			vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(),
				Image,
				ImageType,
				VulkanUtil::FormatToVkFormat(InFormat),
				vk::ComponentMapping(),
				vk::ImageSubresourceRange(
				VulkanUtil::GetImageAspectFromFormat(InFormat), 0, InMipLevels, 0, 1))).value;
		if (!ImageView)
			LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create image view");
	}

	/** Copy initial data */
	if(InCreateInfo.InitialData)
	{
		
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

	ImageView = Device->GetDevice().createImageView(
		vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(),
			Image,
			ImageType,
			VulkanUtil::FormatToVkFormat(InFormat),
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				VulkanUtil::GetImageAspectFromFormat(InFormat), 0, InMipLevels, 0, 1))).value;
	if (!ImageView)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create image view");

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