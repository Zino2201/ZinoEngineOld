#pragma once

#include "VulkanCore.h"
#include "Render/ShaderAttributesManager.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class CVulkanPipeline;
class IRenderSystemUniformBuffer;

/** Automatically create a UBO per variable */
class CVulkanShaderAttributesManager : public IShaderAttributesManager
{
public:
	CVulkanShaderAttributesManager(const SShaderAttributesManagerInfo& InInfos);
	~CVulkanShaderAttributesManager();

	virtual void Set(EShaderStage InStage, const std::string& InName,
		IDeviceResource* InResource) override;

	const std::vector<vk::UniqueDescriptorSet>& GetDescriptorSets() const { return DescriptorSets; }

	virtual void SetUniformBuffer(const std::string& InName,
		void* InData) override;

	virtual const SShaderAttributesManagerInfo& GetInfos() const override { return Infos; }
private:
	CVulkanPipeline* Pipeline;
	std::vector<vk::UniqueDescriptorSet> DescriptorSets;

	/** TODO: Attribute class */
	std::map<SShaderAttribute, IRenderSystemUniformBufferPtr> AttributeBufferMap;
	SShaderAttributesManagerInfo Infos;
	vk::UniqueDescriptorPool DescriptorPool;
};