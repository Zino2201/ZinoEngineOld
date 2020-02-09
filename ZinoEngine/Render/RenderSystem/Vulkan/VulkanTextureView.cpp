#include "VulkanTextureView.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"

CVulkanTextureView::CVulkanTextureView(CVulkanDevice* InDevice, const SRenderSystemTextureViewInfo& InInfos)
	: CRenderSystemTextureView(InInfos), CVulkanDeviceResource(InDevice),
	Texture(static_cast<CVulkanTexture*>(InInfos.Texture))
{
	ImageView = Device->GetDevice().createImageViewUnique(
		vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(),
			Texture->GetImage(),
			VulkanUtil::TextureTypeToVkImageViewType(Texture->GetInfo().Type),
			VulkanUtil::FormatToVkFormat(InInfos.Format),
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				InInfos.ViewType == ETextureViewType::DepthStencil ? vk::ImageAspectFlagBits::eDepth
				: vk::ImageAspectFlagBits::eColor, 0, InInfos.MipLevels, 0, 1))).value;
	if(!ImageView)
		LOG(ELogSeverity::Fatal, "Failed to create image view")
}

CVulkanTextureView::~CVulkanTextureView() {}