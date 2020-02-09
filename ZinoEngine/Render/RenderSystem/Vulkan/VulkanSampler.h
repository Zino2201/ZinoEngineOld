#pragma once

#include "VulkanDeviceResource.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanSampler : public CRenderSystemSampler,
	public CVulkanDeviceResource
{
public:
	CVulkanSampler(CVulkanDevice* InDevice, const SRenderSystemSamplerInfo& InInfos);
	~CVulkanSampler();

	const vk::Sampler& GetSampler() const { return *Sampler; }
private:
	vk::UniqueSampler Sampler;
};