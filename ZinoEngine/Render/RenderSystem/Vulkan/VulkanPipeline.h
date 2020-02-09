#pragma once

#include "VulkanCore.h"
#include "VulkanDeviceResource.h"
#include "VulkanShaderAttributesManager.h"
#include "Render/RenderSystem/RenderSystemResources.h"

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
		const std::vector<SShaderAttribute>& InShaderAttributes);
	virtual ~CVulkanPipeline();

	const vk::Pipeline& GetPipeline() const { return *Pipeline; }
	CVulkanPipelineLayout* GetPipelineLayout() const { return PipelineLayout.get(); }
	const vk::DescriptorSetLayout& GetDescriptorSetLayout(EShaderAttributeFrequency Frequency) 
		{ return *DescriptorSetLayoutMap[Frequency]; }
	const std::vector<vk::DescriptorPoolSize>& GetPoolSizes(EShaderAttributeFrequency Frequency)
		{ return PoolSizeMap[Frequency]; }
	virtual std::shared_ptr<IShaderAttributesManager> CreateShaderAttributesManager(
		EShaderAttributeFrequency InFrequency) const override;
	virtual const std::vector<SShaderAttribute>& GetShaderAttributes() const override { return ShaderAttributes; }
	virtual const std::vector<SShaderAttribute>& GetShaderAttributes(EShaderAttributeFrequency Frequency) 
		override { return AttributeMap[Frequency]; }
protected:
	void Create();
protected:
	vk::UniquePipeline Pipeline;
	std::unique_ptr<CVulkanPipelineLayout> PipelineLayout;
	std::map<EShaderAttributeFrequency, vk::UniqueDescriptorSetLayout> DescriptorSetLayoutMap;
	std::map<EShaderAttributeFrequency, std::vector<vk::DescriptorPoolSize>> PoolSizeMap;
	std::vector<SShaderAttribute> ShaderAttributes;
	std::map<EShaderAttributeFrequency, std::vector<SShaderAttribute>> AttributeMap;

	friend class CVulkanShaderAttributesManager;
};

/** Render pipeline */
class CVulkanGraphicsPipeline : public CVulkanPipeline,
	public IRenderSystemGraphicsPipeline
{
public:
	CVulkanGraphicsPipeline(CVulkanDevice* InDevice,
		const SRenderSystemGraphicsPipelineInfos& InInfos);
	~CVulkanGraphicsPipeline();

	virtual std::shared_ptr<IShaderAttributesManager> CreateShaderAttributesManager(
		EShaderAttributeFrequency InFrequency) const override 
	{ return CVulkanPipeline::CreateShaderAttributesManager(InFrequency);}
	virtual const std::vector<SShaderAttribute>& GetShaderAttributes() const override { return ShaderAttributes; }
	virtual const std::vector<SShaderAttribute>& GetShaderAttributes(EShaderAttributeFrequency Frequency)
		override { return CVulkanPipeline::GetShaderAttributes(Frequency); }
};