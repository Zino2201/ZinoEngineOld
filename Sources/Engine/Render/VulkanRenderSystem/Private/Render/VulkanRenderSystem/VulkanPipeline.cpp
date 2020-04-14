#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanRenderSystemContext.h"
#include <unordered_set>

CVulkanPipeline::CVulkanPipeline(CVulkanDevice* InDevice,
	const std::vector<SRSPipelineShaderStage>& InShaderStages,
	const SRSResourceCreateInfo& InCreateInfo) : CVulkanDeviceResource(InDevice),
	CRSPipeline(InShaderStages, InCreateInfo)
{

}

CVulkanGraphicsPipeline::CVulkanGraphicsPipeline(CVulkanDevice* InDevice,
	const std::vector<SRSPipelineShaderStage>& InShaderStages,
	const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
	const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
	const SRSRenderPass& InRenderPass,
	const SRSBlendState& InBlendState,
	const SRSRasterizerState& InRasterizerState,
	const SRSDepthStencilState& InDepthStencilState,
	const SRSResourceCreateInfo& InCreateInfo)
	: CVulkanPipeline(InDevice, InShaderStages, InCreateInfo),
	CRSGraphicsPipeline(InShaderStages, InBindingDescriptions,
		InAttributeDescriptions, 
		InRenderPass,
		InBlendState, InRasterizerState,
		InDepthStencilState, InCreateInfo)
{
	std::unordered_map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> Bindings;
	std::unordered_set<uint64_t> AddedParametersHash;

	for(const auto& Stage : InShaderStages)
	{
		for (const auto& Parameter : Stage.Shader->GetShaderParameterMap().GetParameters())
		{
			uint64_t HashParameter = SShaderParameterHash()(Parameter);

			if (AddedParametersHash.count(HashParameter) == 0)
			{
				Bindings[Parameter.Set].emplace_back(
					Parameter.Binding,
					VulkanUtil::ShaderParameterTypeToVkDescriptorType(Parameter.Type),
					Parameter.Count,
					VulkanUtil::ShaderStageToVkShaderStage(Stage.Stage));
			}
			else
			{
				for (auto& Binding : Bindings[Parameter.Set])
				{
					if(Binding.binding == Parameter.Binding)
					{
						Binding.stageFlags |= VulkanUtil::ShaderStageToVkShaderStage(Stage.Stage);
						break;
					}
				}
			}
		}
	}

	SVulkanPipelineLayoutDesc Desc;
	Desc.SetLayoutBindings.reserve(Bindings.size());
	for(const auto& [Set, SetBindings] : Bindings)
	{
		SVulkanDescriptorSetBindings LayoutSetBindings;
		LayoutSetBindings.Bindings = SetBindings;
		Desc.SetLayoutBindings.push_back(LayoutSetBindings);
	}

	/**
	 * Create actual pipeline
	 */
	vk::GraphicsPipelineCreateInfo CreateInfo;

	Layout = Device->GetPipelineLayoutMgr()->GetPipelineLayout(Desc);

	vk::RenderPass RenderPass = Device->GetContext()->GetRenderPassMgr().GetRenderPass(
		InRenderPass);

	CreateInfo.setRenderPass(RenderPass);
	CreateInfo.setLayout(Layout->GetPipelineLayout());
	
	/**
	 * Stages
	 */
	std::vector<vk::PipelineShaderStageCreateInfo> Stages;
	Stages.reserve(InShaderStages.size());
	for (const auto& ShaderStage : InShaderStages)
	{
		Stages.emplace_back(
			vk::PipelineShaderStageCreateFlags(),
			VulkanUtil::ShaderStageToVkShaderStage(ShaderStage.Stage),
			static_cast<CVulkanShader*>(ShaderStage.Shader)->GetShaderModule(),
			ShaderStage.EntryPoint);
	}

	CreateInfo.setStageCount(static_cast<uint32_t>(Stages.size()));
	CreateInfo.setPStages(Stages.data());

	/**
	 * Vertex input state
	 */
	std::vector<vk::VertexInputBindingDescription> InputAttributes;
	InputAttributes.reserve(InBindingDescriptions.size());
	for (const auto& BindingDescription : InBindingDescriptions)
	{
		InputAttributes.emplace_back(
			BindingDescription.Binding,
			BindingDescription.Stride,
			VulkanUtil::VertexInputRateToVkVertexInputRate(BindingDescription.InputRate));
	}

	/** Vertex attributes */
	std::vector<vk::VertexInputAttributeDescription> Attributes;
	Attributes.reserve(InAttributeDescriptions.size());
	for (const SVertexInputAttributeDescription& Attribute : InAttributeDescriptions)
	{
		Attributes.emplace_back(
			Attribute.Location,
			Attribute.Binding,
			VulkanUtil::FormatToVkFormat(Attribute.Format),
			Attribute.Offset);
	}

	vk::PipelineVertexInputStateCreateInfo InputState = vk::PipelineVertexInputStateCreateInfo(
		vk::PipelineVertexInputStateCreateFlags(),
		static_cast<uint32_t>(InputAttributes.size()),
		InputAttributes.data(),
		static_cast<uint32_t>(Attributes.size()),
		Attributes.data());
	CreateInfo.setPVertexInputState(&InputState);

	/**
	 * Dynamic state
	 */
	std::array<vk::DynamicState, 2> DynamicStates =
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
	};

	vk::PipelineInputAssemblyStateCreateInfo InputAssemblyState = 
		vk::PipelineInputAssemblyStateCreateInfo(
			vk::PipelineInputAssemblyStateCreateFlags(),
			vk::PrimitiveTopology::eTriangleList,
			VK_FALSE);
	CreateInfo.setPInputAssemblyState(&InputAssemblyState);

	/** Temporary viewport & scissor, should be set using SetViewports */
	vk::Viewport Viewport;
	vk::Rect2D Scissor;
	vk::PipelineViewportStateCreateInfo ViewportState(
		vk::PipelineViewportStateCreateFlags(),
		1,
		&Viewport,
		1,
		&Scissor);
	CreateInfo.setPViewportState(&ViewportState);

	/** Rasterizer state */
	vk::PipelineRasterizationStateCreateInfo RasterizerState(
		vk::PipelineRasterizationStateCreateFlags(),
		InRasterizerState.bEnableDepthClamp ? VK_TRUE : VK_FALSE,
		InRasterizerState.bEnableRasterizerDiscard ? VK_TRUE : VK_FALSE,
		VulkanUtil::PolygonModeToVkPolygonMode(InRasterizerState.PolygonMode),
		VulkanUtil::CullModeToVkCullMode(InRasterizerState.CullMode),
		VulkanUtil::FrontFaceToVkFrontFace(InRasterizerState.FrontFace),
		VK_FALSE,
		0.f,
		0.f,
		0.f,
		1.f);
	CreateInfo.setPRasterizationState(&RasterizerState);

	/** Multisampling */
	vk::PipelineMultisampleStateCreateInfo MultisamplingState(
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1,
		VK_FALSE,
		1.f,
		nullptr,
		VK_FALSE,
		VK_FALSE);
	CreateInfo.setPMultisampleState(&MultisamplingState);

	vk::PipelineDepthStencilStateCreateInfo DepthState(
		vk::PipelineDepthStencilStateCreateFlags(),
		InDepthStencilState.bEnableDepthTest ? VK_TRUE : VK_FALSE,
		InDepthStencilState.bEnableDepthWrite ? VK_TRUE : VK_FALSE,
		VulkanUtil::ComparisonOpToVkCompareOp(InDepthStencilState.DepthCompareOp),
		InDepthStencilState.bDepthBoundsTestEnable ? VK_TRUE : VK_FALSE,
		InDepthStencilState.bStencilTestEnable ? VK_TRUE : VK_FALSE,
		vk::StencilOpState(
			VulkanUtil::StencilOpToVkStencilOp(InDepthStencilState.FrontFace.FailOp),
			VulkanUtil::StencilOpToVkStencilOp(InDepthStencilState.FrontFace.PassOp),
			VulkanUtil::StencilOpToVkStencilOp(InDepthStencilState.FrontFace.DepthFailOp),
			VulkanUtil::ComparisonOpToVkCompareOp(InDepthStencilState.FrontFace.CompareOp)),
		vk::StencilOpState(
			VulkanUtil::StencilOpToVkStencilOp(InDepthStencilState.BackFace.FailOp),
			VulkanUtil::StencilOpToVkStencilOp(InDepthStencilState.BackFace.PassOp),
			VulkanUtil::StencilOpToVkStencilOp(InDepthStencilState.BackFace.DepthFailOp),
			VulkanUtil::ComparisonOpToVkCompareOp(InDepthStencilState.BackFace.CompareOp))
	);
	CreateInfo.setPDepthStencilState(&DepthState);

	/** Color blending */
	vk::PipelineColorBlendAttachmentState ColorBlendAttachment(
		InBlendState.bEnableBlend ? VK_TRUE : VK_FALSE,
		VulkanUtil::BlendFactorToVkBlendFactor(InBlendState.SrcColor),
		VulkanUtil::BlendFactorToVkBlendFactor(InBlendState.DstColor),
		VulkanUtil::BlendOpToVkBlendOp(InBlendState.ColorOp),
		VulkanUtil::BlendFactorToVkBlendFactor(InBlendState.SrcAlpha),
		VulkanUtil::BlendFactorToVkBlendFactor(InBlendState.DstAlpha),
		VulkanUtil::BlendOpToVkBlendOp(InBlendState.AlphaOp),
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	/**
	 * Color blend state
	 */
	vk::PipelineColorBlendStateCreateInfo ColorBlendState(
		vk::PipelineColorBlendStateCreateFlags(),
		VK_FALSE,
		vk::LogicOp::eCopy,
		1,
		&ColorBlendAttachment);
	CreateInfo.setPColorBlendState(&ColorBlendState);

	/**
	 * Dynamic states
	 */
	vk::PipelineDynamicStateCreateInfo DynamicState = vk::PipelineDynamicStateCreateInfo(
		vk::PipelineDynamicStateCreateFlags(),
		static_cast<uint32_t>(DynamicStates.size()),
		DynamicStates.data());
	CreateInfo.setPDynamicState(&DynamicState);

	Pipeline = Device->GetDevice().createGraphicsPipelineUnique(vk::PipelineCache(),
		CreateInfo).value;
	if (!Pipeline)
		LOG(ELogSeverity::Fatal, VulkanRS, "Failed to create pipeline");
}

vk::DescriptorType VulkanUtil::ShaderParameterTypeToVkDescriptorType(
	const EShaderParameterType& InType)
{
	switch(InType)
	{
	default:
	case EShaderParameterType::UniformBuffer:
		return vk::DescriptorType::eUniformBuffer;
	case EShaderParameterType::CombinedImageSampler:
		return vk::DescriptorType::eCombinedImageSampler;
	case EShaderParameterType::StorageBuffer:
		return vk::DescriptorType::eStorageBuffer;
	}
}

CRSGraphicsPipeline* CVulkanRenderSystem::CreateGraphicsPipeline(
	const std::vector<SRSPipelineShaderStage>& InShaderStages,
	const std::vector<SVertexInputBindingDescription>& InBindingDescriptions,
	const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
	const SRSRenderPass& InRenderPass,
	const SRSBlendState& InBlendState,
	const SRSRasterizerState& InRasterizerState,
	const SRSDepthStencilState& InDepthStencilState,
	const SRSResourceCreateInfo& InCreateInfo) const
{
	return new CVulkanGraphicsPipeline(Device.get(),
		InShaderStages,
		InBindingDescriptions,
		InAttributeDescriptions,
		InRenderPass,
		InBlendState,
		InRasterizerState,
		InDepthStencilState,
		InCreateInfo);
}

vk::CompareOp VulkanUtil::ComparisonOpToVkCompareOp(const ERSComparisonOp& InOp)
{
	switch (InOp)
	{
	default:
	case ERSComparisonOp::Never:
		return vk::CompareOp::eNever;
	case ERSComparisonOp::Always:
		return vk::CompareOp::eAlways;
	case ERSComparisonOp::Less:
		return vk::CompareOp::eLess;
	case ERSComparisonOp::Equal:
		return vk::CompareOp::eEqual;
	case ERSComparisonOp::LessOrEqual:
		return vk::CompareOp::eLess;
	case ERSComparisonOp::Greater:
		return vk::CompareOp::eGreater;
	case ERSComparisonOp::NotEqual:
		return vk::CompareOp::eNotEqual;
	case ERSComparisonOp::GreaterOrEqual:
		return vk::CompareOp::eGreaterOrEqual;
	}
}

/** Blend */
vk::BlendFactor VulkanUtil::BlendFactorToVkBlendFactor(EBlendFactor InFactor)
{
	switch (InFactor)
	{
	default:
	case EBlendFactor::Zero:
		return vk::BlendFactor::eZero;
	case EBlendFactor::One:
		return vk::BlendFactor::eOne;
	case EBlendFactor::SrcAlpha:
		return vk::BlendFactor::eSrcAlpha;
	case EBlendFactor::OneMinusSrcAlpha:
		return vk::BlendFactor::eOneMinusSrcAlpha;
	}
}

vk::BlendOp VulkanUtil::BlendOpToVkBlendOp(EBlendOp InOp)
{
	switch (InOp)
	{
	default:
	case EBlendOp::Add:
		return vk::BlendOp::eAdd;
	}
}

vk::StencilOp VulkanUtil::StencilOpToVkStencilOp(EStencilOp InOp)
{
	switch (InOp)
	{
	default:
	case EStencilOp::Keep: return vk::StencilOp::eKeep;
	case EStencilOp::Zero: return vk::StencilOp::eZero;
	case EStencilOp::Replace: return vk::StencilOp::eReplace;
	case EStencilOp::IncrementAndClamp: return vk::StencilOp::eIncrementAndClamp;
	case EStencilOp::DecrementAndClamp: return vk::StencilOp::eDecrementAndClamp;
	case EStencilOp::Invert: return vk::StencilOp::eInvert;
	case EStencilOp::IncrementAndWrap: return vk::StencilOp::eIncrementAndWrap;
	case EStencilOp::DecrementAndWrap: return vk::StencilOp::eDecrementAndWrap;
	}
}

vk::PolygonMode VulkanUtil::PolygonModeToVkPolygonMode(EPolygonMode InPolygonMode)
{
	switch (InPolygonMode)
	{
	default:
	case EPolygonMode::Fill: return vk::PolygonMode::eFill;
	case EPolygonMode::Line: return vk::PolygonMode::eLine;
	case EPolygonMode::Point: return vk::PolygonMode::ePoint;
	}
}

vk::CullModeFlags VulkanUtil::CullModeToVkCullMode(ECullMode InCullMode)
{
	switch (InCullMode)
	{
	default:
	case ECullMode::None: return vk::CullModeFlagBits::eNone;
	case ECullMode::Back: return vk::CullModeFlagBits::eBack;
	case ECullMode::Front: return vk::CullModeFlagBits::eFront;
	case ECullMode::FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
	}
}

vk::FrontFace VulkanUtil::FrontFaceToVkFrontFace(EFrontFace InFrontFace)
{
	switch (InFrontFace)
	{
	default:
	case EFrontFace::Clockwise: return vk::FrontFace::eClockwise;
	case EFrontFace::CounterClockwise: return vk::FrontFace::eCounterClockwise;
	}
}

vk::VertexInputRate VulkanUtil::VertexInputRateToVkVertexInputRate(const EVertexInputRate& InRate)
{
	switch(InRate)
	{
	default:
	case EVertexInputRate::Vertex:
		return vk::VertexInputRate::eVertex;
	case EVertexInputRate::Instance:
		return vk::VertexInputRate::eInstance;
	}
}