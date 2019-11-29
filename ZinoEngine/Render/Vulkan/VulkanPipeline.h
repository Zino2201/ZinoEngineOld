#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"
#include "Render/Pipeline.h"

class CVulkanDevice;
class CVulkanShader;
class CVulkanPipelineLayout;

/**
 * Vulkan pipeline
 */
class CVulkanPipeline : public IPipeline, 
	public CVulkanDeviceResource
{
public:
	CVulkanPipeline(CVulkanDevice* InDevice);
	virtual ~CVulkanPipeline();

	const vk::Pipeline& GetPipeline() const { return *Pipeline; }
protected:
	vk::UniquePipeline Pipeline;
	std::unique_ptr<CVulkanPipelineLayout> PipelineLayout;
};

/** Render pipeline */
class CVulkanGraphicsPipeline : public IGraphicsPipeline,
	public CVulkanPipeline
{
public:
	CVulkanGraphicsPipeline(CVulkanDevice* InDevice,
		IShader* InVertexShader,
		IShader* InFragmentShader,
		const SVertexInputBindingDescription& InBindingDescription,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions);
	~CVulkanGraphicsPipeline();
};