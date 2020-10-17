#pragma once

#include "Pipeline.h"

namespace ze
{

/**
 * Represents a graphics pipeline
 */
struct SRSGraphicsPipeline
{
    std::vector<SRSPipelineShaderStage> ShaderStages;
    std::vector<SVertexInputBindingDescription> BindingDescriptions;
    std::vector<SVertexInputAttributeDescription> AttributeDescriptions;
    SRSBlendState BlendState;
    SRSRasterizerState RasterizerState;
    SRSDepthStencilState DepthStencilState;

    SRSGraphicsPipeline() = default;
    SRSGraphicsPipeline(const std::vector<SRSPipelineShaderStage>& InStages,
        const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
        const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
        const SRSBlendState& InBlendState,
        const SRSRasterizerState& InRasterizerState,
        const SRSDepthStencilState& InDepthStencilState) : ShaderStages(InStages),
        BindingDescriptions(InBindingDescriptions), AttributeDescriptions(InAttributeDescriptions),
        BlendState(InBlendState), RasterizerState(InRasterizerState), 
        DepthStencilState(InDepthStencilState) {}

    bool operator==(const SRSGraphicsPipeline& InOther) const
    {
        return ShaderStages == InOther.ShaderStages &&
            BindingDescriptions == InOther.BindingDescriptions &&
            AttributeDescriptions == InOther.AttributeDescriptions &&
            BlendState == InOther.BlendState &&
            RasterizerState == InOther.RasterizerState &&
            DepthStencilState == InOther.DepthStencilState;
    }
};

struct SRSGraphicsPipelineHash
{
    uint64_t operator()(const SRSGraphicsPipeline& InPipeline) const
    {
        uint64_t Hash = 0;

        for(const auto& Stage : InPipeline.ShaderStages)
            hash_combine<SRSPipelineShaderStage, SRSPipelineShaderStageHash>(Hash, Stage);

		for (const auto& BindingDescription : InPipeline.BindingDescriptions)
			hash_combine<SVertexInputBindingDescription, SVertexInputBindingDescriptionHash>(Hash,
                BindingDescription);

		for (const auto& AttributeDescription : InPipeline.AttributeDescriptions)
			hash_combine<SVertexInputAttributeDescription, SVertexInputAttributeDescriptionHash>(Hash,
				AttributeDescription);

        hash_combine<SRSBlendState, SRSBlendStateHash>(Hash, InPipeline.BlendState);
        hash_combine<SRSRasterizerState, SRSRasterizerStateHash>(Hash, InPipeline.RasterizerState);
        hash_combine<SRSDepthStencilState, SRSDepthStencilStateHash>(Hash, 
            InPipeline.DepthStencilState);

        return Hash;
    }
};

}