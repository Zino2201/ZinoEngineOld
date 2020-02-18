#pragma once

#include "VulkanCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "VulkanDeviceResource.h"

class CVulkanDevice;
class CVulkanShader;
class CVulkanPipelineLayout;

/**
 * Vulkan pipeline
 */
class CVulkanPipeline : public CRenderSystemPipeline, 
	public CVulkanDeviceResource
{
public:
	CVulkanPipeline(CVulkanDevice* InDevice,
		const std::vector<SShaderParameter>& InShaderParameters);
	virtual ~CVulkanPipeline();

	const vk::Pipeline& GetPipeline() const { return *Pipeline; }
	CVulkanPipelineLayout* GetPipelineLayout() const { return PipelineLayout.get(); }
protected:
	void Create();
protected:
	vk::UniquePipeline Pipeline;
	std::unique_ptr<CVulkanPipelineLayout> PipelineLayout;
	std::map<uint32_t, vk::UniqueDescriptorSetLayout> SetLayouts;
	std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> SetLayoutBindings;
	std::vector<SShaderParameter> ShaderParameters;
};

/** Render pipeline */
class CVulkanGraphicsPipeline : public CVulkanPipeline,
	public IRenderSystemGraphicsPipeline
{
public:
	CVulkanGraphicsPipeline(CVulkanDevice* InDevice,
		const SRenderSystemGraphicsPipelineInfos& InInfos);
	~CVulkanGraphicsPipeline();
};