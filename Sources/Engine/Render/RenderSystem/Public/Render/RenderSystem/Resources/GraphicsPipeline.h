#pragma once

#include "Pipeline.h"

namespace ZE
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
            HashCombine<SRSPipelineShaderStage, SRSPipelineShaderStageHash>(Hash, Stage);

		for (const auto& BindingDescription : InPipeline.BindingDescriptions)
			HashCombine<SVertexInputBindingDescription, SVertexInputBindingDescriptionHash>(Hash,
                BindingDescription);

		for (const auto& AttributeDescription : InPipeline.AttributeDescriptions)
			HashCombine<SVertexInputAttributeDescription, SVertexInputAttributeDescriptionHash>(Hash,
				AttributeDescription);

        HashCombine<SRSBlendState, SRSBlendStateHash>(Hash, InPipeline.BlendState);
        HashCombine<SRSRasterizerState, SRSRasterizerStateHash>(Hash, InPipeline.RasterizerState);
        HashCombine<SRSDepthStencilState, SRSDepthStencilStateHash>(Hash, 
            InPipeline.DepthStencilState);

        return Hash;
    }
};

}