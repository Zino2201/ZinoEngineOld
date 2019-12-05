#include "VulkanSampler.h"
#include "VulkanDevice.h"

CVulkanSampler::CVulkanSampler(CVulkanDevice* InDevice, const SSamplerInfo& InInfos) 
	: ISampler(InInfos), CVulkanDeviceResource(InDevice)
{
	Sampler = Device->GetDevice().createSamplerUnique(
		vk::SamplerCreateInfo(vk::SamplerCreateFlags(),
			VulkanUtil::SamplerFilterToVkFilter(InInfos.MagFilter),
			VulkanUtil::SamplerFilterToVkFilter(InInfos.MinFilter),
			VulkanUtil::SamplerFilterToVkSamplerMipmapMode(InInfos.MipFilter),
			VulkanUtil::SamplerAddressModeToVkSamplerAddressMode(InInfos.AddressModeU),
			VulkanUtil::SamplerAddressModeToVkSamplerAddressMode(InInfos.AddressModeV),
			VulkanUtil::SamplerAddressModeToVkSamplerAddressMode(InInfos.AddressModeW),
			InInfos.MipLODBias,
			InInfos.MaxAnisotropy > 0.f ? VK_TRUE : VK_FALSE,
			InInfos.MaxAnisotropy,
			VK_FALSE, /* Enable compare */
			VulkanUtil::ComparisonOpToVkCompareOp(InInfos.ComparisonOp),
			InInfos.MinLOD,
			InInfos.MaxLOD,
			vk::BorderColor::eIntOpaqueBlack,
			VK_FALSE)).value;
	if(!Sampler)
		LOG(ELogSeverity::Fatal, "Failed to create sampler")
}

CVulkanSampler::~CVulkanSampler() {}