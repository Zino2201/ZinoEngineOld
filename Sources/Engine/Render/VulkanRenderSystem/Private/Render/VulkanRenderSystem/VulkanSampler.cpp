#include "VulkanSampler.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"

CVulkanSampler::CVulkanSampler(CVulkanDevice& InDevice, 
	const SRSSamplerCreateInfo& InSamplerCreateInfo) : CRSSampler(InSamplerCreateInfo),
	Device(InDevice)
{
	Sampler = Device.GetDevice().createSampler(
		vk::SamplerCreateInfo(
			vk::SamplerCreateFlags(),
			VulkanUtil::FilterToVkFilter(InSamplerCreateInfo.MagFilter),
			VulkanUtil::FilterToVkFilter(InSamplerCreateInfo.MinFilter),
			VulkanUtil::FilterToVkSamplerMipMapMode(InSamplerCreateInfo.MipMapMode),
			VulkanUtil::AddressModeToVkSamplerAddressMode(InSamplerCreateInfo.AddressModeU),
			VulkanUtil::AddressModeToVkSamplerAddressMode(InSamplerCreateInfo.AddressModeV),
			VulkanUtil::AddressModeToVkSamplerAddressMode(InSamplerCreateInfo.AddressModeW),
			InSamplerCreateInfo.MipLodBias,
			InSamplerCreateInfo.bAnistropyEnable,
			InSamplerCreateInfo.MaxAnistropy,
			InSamplerCreateInfo.CompareOp == ERSComparisonOp::Never ? false : true,
			VulkanUtil::ComparisonOpToVkCompareOp(InSamplerCreateInfo.CompareOp),
			InSamplerCreateInfo.MinLOD,
			InSamplerCreateInfo.MaxLOD)).value;
	if(!Sampler)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create Vulkan sampler");
}

CVulkanSampler::~CVulkanSampler()
{
	Device.GetDeferredDestructionMgr().Enqueue<vk::Sampler, VkSampler>(
		CVulkanDeferredDestructionManager::EHandleType::Sampler,
		Sampler);
}

CRSSampler* CVulkanRenderSystem::CreateSampler(const SRSSamplerCreateInfo& InCreateInfo) const
{
	return new CVulkanSampler(*Device.get(), InCreateInfo);
}

vk::Filter VulkanUtil::FilterToVkFilter(const ERSFilter& InFilter)
{
	switch(InFilter)
	{
	default:
	case ERSFilter::Nearest:
		return vk::Filter::eNearest;
	case ERSFilter::Linear:
		return vk::Filter::eLinear;
	}
}

vk::SamplerMipmapMode VulkanUtil::FilterToVkSamplerMipMapMode(const ERSFilter& InFilter)
{
	switch (InFilter)
	{
	default:
	case ERSFilter::Nearest:
		return vk::SamplerMipmapMode::eNearest;
	case ERSFilter::Linear:
		return vk::SamplerMipmapMode::eLinear;
	}
}

vk::SamplerAddressMode VulkanUtil::AddressModeToVkSamplerAddressMode(
	const ERSSamplerAddressMode& InAddressMode)
{
	switch(InAddressMode)
	{
	default:
	case ERSSamplerAddressMode::Repeat:
		return vk::SamplerAddressMode::eRepeat;
	case ERSSamplerAddressMode::MirroredRepeat:
		return vk::SamplerAddressMode::eMirroredRepeat;
	case ERSSamplerAddressMode::ClampToEdge:
		return vk::SamplerAddressMode::eClampToEdge;
	case ERSSamplerAddressMode::ClampToBorder:
		return vk::SamplerAddressMode::eClampToBorder;
	}
}