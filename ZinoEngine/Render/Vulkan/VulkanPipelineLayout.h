#pragma once

#include "VulkanDeviceResource.h"

class CVulkanPipelineLayout : public CVulkanDeviceResource
{
public:
	CVulkanPipelineLayout(CVulkanDevice* InDevice);
	~CVulkanPipelineLayout();

	const vk::PipelineLayout& GetPipelineLayout() const { return *PipelineLayout; }
private:
	vk::UniquePipelineLayout PipelineLayout;
};