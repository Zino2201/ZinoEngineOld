#pragma once

#include "VulkanCore.h"
#include "Render/ShaderAttributesManager.h"

class CVulkanPipeline;

class CVulkanShaderAttributesManager : public IShaderAttributesManager
{
public:
	CVulkanShaderAttributesManager(const SShaderAttributesManagerInfo& InInfos);
	~CVulkanShaderAttributesManager();

	virtual void Set(EShaderStage InStage, const std::string& InName,
		IDeviceResource* InResource) override;

	const std::vector<vk::UniqueDescriptorSet>& GetDescriptorSets() const { return DescriptorSets; }
private:
	CVulkanPipeline* Pipeline;
	std::vector<vk::UniqueDescriptorSet> DescriptorSets;
};