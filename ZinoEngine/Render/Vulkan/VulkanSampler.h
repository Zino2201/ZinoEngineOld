#pragma once

#include "VulkanDeviceResource.h"
#include "Render/Sampler.h"

class CVulkanSampler : public ISampler,
	public CVulkanDeviceResource
{
public:
	CVulkanSampler(CVulkanDevice* InDevice, const SSamplerInfo& InInfos);
	~CVulkanSampler();

	const vk::Sampler& GetSampler() const { return *Sampler; }
private:
	vk::UniqueSampler Sampler;
};