#include "VulkanTextureView.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"

CVulkanTextureView::CVulkanTextureView(CVulkanDevice* InDevice, const STextureViewInfo& InInfos)
	: ITextureView(InInfos), CVulkanDeviceResource(InDevice),
	Texture(static_cast<CVulkanTexture*>(InInfos.Texture))
{
	ImageView = Device->GetDevice().createImageViewUnique(
		vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(),
			Texture->GetImage(),
			VulkanUtil::TextureViewTypeViewToVkImageViewType(InInfos.ViewType),
			VulkanUtil::FormatToVkFormat(InInfos.Format),
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))).value;
	if(!ImageView)
		LOG(ELogSeverity::Fatal, "Failed to create image view")
}

CVulkanTextureView::~CVulkanTextureView() {}

void CVulkanTextureView::SetSampler(const std::shared_ptr<ISampler>& InSampler)
{
	Sampler = std::static_pointer_cast<CVulkanSampler>(InSampler);
}

ISampler* CVulkanTextureView::GetSampler() const 
{ 
	return Sampler.get(); 
}