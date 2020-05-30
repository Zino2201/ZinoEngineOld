#pragma once

#include "VulkanCore.h"

class CVulkanSampler : public CRSSampler
{
public:
	CVulkanSampler(CVulkanDevice& InDevice, const SRSSamplerCreateInfo& InSamplerCreateInfo);
	~CVulkanSampler();

	const vk::Sampler& GetSampler() const { return Sampler; }
private:
	CVulkanDevice& Device;
	vk::Sampler Sampler;
};