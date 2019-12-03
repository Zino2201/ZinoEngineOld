#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"
#include "Render/Pipeline.h"
#include "VulkanShaderAttributesManager.h"

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
	CVulkanPipeline(CVulkanDevice* InDevice,
		const std::vector<SShaderAttribute>& InShaderAttributes);
	virtual ~CVulkanPipeline();

	const vk::Pipeline& GetPipeline() const { return *Pipeline; }
	CVulkanPipelineLayout* GetPipelineLayout() const { return PipelineLayout.get(); }
	const vk::DescriptorPool& GetDescriptorPool() const { return *DescriptorPool; }
	const vk::DescriptorSetLayout& GetDescriptorSetLayout() const { return *DescriptorSetLayout; }
	virtual IShaderAttributesManager* GetShaderAttributesManager() const override { return ShaderAttributesManager.get(); }
protected:
	void Create();
protected:
	vk::UniquePipeline Pipeline;
	std::unique_ptr<CVulkanPipelineLayout> PipelineLayout;
	vk::UniqueDescriptorSetLayout DescriptorSetLayout;
	vk::UniqueDescriptorPool DescriptorPool;
	std::unique_ptr<CVulkanShaderAttributesManager> ShaderAttributesManager;
	std::vector<SShaderAttribute> ShaderAttributes;

	friend class CVulkanShaderAttributesManager;
};

/** Render pipeline */
class CVulkanGraphicsPipeline : public CVulkanPipeline,
	public IGraphicsPipeline
{
public:
	CVulkanGraphicsPipeline(CVulkanDevice* InDevice,
		const SGraphicsPipelineInfos& InInfos);
	~CVulkanGraphicsPipeline();

	virtual IShaderAttributesManager* GetShaderAttributesManager() const override { return ShaderAttributesManager.get(); }
};