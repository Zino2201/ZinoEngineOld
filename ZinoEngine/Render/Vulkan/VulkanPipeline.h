#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"

class CVulkanDevice;
class CVulkanShader;
class CVulkanPipelineLayout;

/**
 * Vulkan pipeline
 */
class CVulkanPipeline : public CVulkanDeviceResource
{
public:
	CVulkanPipeline(CVulkanDevice* InDevice);
	virtual ~CVulkanPipeline();
protected:
	vk::UniquePipeline Pipeline;
	std::unique_ptr<CVulkanPipelineLayout> PipelineLayout;
};

/** Render pipeline */
class CVulkanRenderPipeline : public CVulkanPipeline
{
public:
	CVulkanRenderPipeline(CVulkanDevice* InDevice,
		const std::vector<CVulkanShader*>& InShaders);
	~CVulkanRenderPipeline();
};