#pragma once

#include "VulkanCore.h"

/** Fwd declares */
class CVulkanPipelineLayout;

/**
 * Basic vulkan pipeline
 */
class CVulkanPipeline : public CVulkanDeviceResource, 
    public CRSPipeline
{
public:
    CVulkanPipeline(CVulkanDevice* InDevice,
        const std::vector<SRSPipelineShaderStage>& InShaderStages,
		const SRSResourceCreateInfo& InCreateInfo);

    virtual const vk::Pipeline& GetPipeline() const { return *Pipeline; }
    virtual CVulkanPipelineLayout* GetPipelineLayout() const { return Layout.get(); }
protected:
    vk::UniquePipeline Pipeline;
    boost::intrusive_ptr<CVulkanPipelineLayout> Layout;
};

/**
 * Graphics pipeline
 */
class CVulkanGraphicsPipeline : public CVulkanPipeline,
    public CRSGraphicsPipeline
{
public:
    CVulkanGraphicsPipeline(CVulkanDevice* InDevice,
		const std::vector<SRSPipelineShaderStage>& InShaderStages,
		const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
        const SRSRenderPass& InRenderPass,
		const SRSBlendState& InBlendState,
		const SRSRasterizerState& InRasterizerState,
		const SRSDepthStencilState& InDepthStencilState,
		const SRSResourceCreateInfo& InCreateInfo);
};