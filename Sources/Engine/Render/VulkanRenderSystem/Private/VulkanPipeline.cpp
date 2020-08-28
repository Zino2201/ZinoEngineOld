#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanRenderSystem.h"
#include "VulkanRenderSystemContext.h"
#include <unordered_set>

CVulkanPipelineManager::CVulkanPipelineManager(CVulkanDevice& InDevice) : Device(InDevice) {}

CVulkanGraphicsPipeline* CVulkanPipelineManager::GetOrCreateGraphicsPipeline(
	const SRSGraphicsPipeline& InPipeline,
	const vk::RenderPass& InRenderPass)
{
	SGraphicsEntry Entry;
	Entry.Pipeline = InPipeline;
	Entry.RenderPass = InRenderPass;

	auto Result = GraphicsPipelines.find(Entry);
	if(Result != GraphicsPipelines.end())
		return Result->second.get();

	TOwnerPtr<CVulkanGraphicsPipeline> Pipeline = new CVulkanGraphicsPipeline(
		Device, InPipeline, InRenderPass);
	
	GraphicsPipelines.insert({ Entry, std::unique_ptr<CVulkanGraphicsPipeline>(Pipeline) });

	return Pipeline;
}

CVulkanPipeline::CVulkanPipeline(CVulkanDevice& InDevice) : CVulkanDeviceResource(InDevice)
{

}

CVulkanGraphicsPipeline::CVulkanGraphicsPipeline(CVulkanDevice& InDevice,
	const SRSGraphicsPipeline& InGraphicsPipeline,
	const vk::RenderPass& InRenderPass)
	: CVulkanPipeline(InDevice)
{
	std::unordered_map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> Bindings;
	std::unordered_set<uint64_t> AddedParametersHash;

	for(const auto& Stage : InGraphicsPipeline.ShaderStages)
	{
		for (const auto& Parameter : Stage.Shader->GetCreateInfo().ParameterMap.GetParameters())
		{
			uint64_t HashParameter = SShaderParameterHash()(Parameter);

			if (AddedParametersHash.count(HashParameter) == 0)
			{
				Bindings[Parameter.Set].emplace_back(
					Parameter.Binding,
					VulkanUtil::ShaderParameterTypeToVkDescriptorType(Parameter.Type),
					Parameter.Count,
					VulkanUtil::ShaderStageToVkShaderStage(Stage.Stage));

				AddedParametersHash.insert(HashParameter);
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
	Layout = Device.GetPipelineLayoutMgr().GetPipelineLayout(Desc);

	CreateInfo.setRenderPass(InRenderPass);
	CreateInfo.setLayout(Layout->GetPipelineLayout());
	
	/**
	 * Stages
	 */
	std::vector<vk::PipelineShaderStageCreateInfo> Stages;
	Stages.reserve(InGraphicsPipeline.ShaderStages.size());
	for (const auto& ShaderStage : InGraphicsPipeline.ShaderStages)
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
	InputAttributes.reserve(InGraphicsPipeline.BindingDescriptions.size());
	for (const auto& BindingDescription : InGraphicsPipeline.BindingDescriptions)
	{
		InputAttributes.emplace_back(
			BindingDescription.Binding,
			BindingDescription.Stride,
			VulkanUtil::VertexInputRateToVkVertexInputRate(BindingDescription.InputRate));
	}

	/** Vertex attributes */
	std::vector<vk::VertexInputAttributeDescription> Attributes;
	Attributes.reserve(InGraphicsPipeline.AttributeDescriptions.size());
	for (const SVertexInputAttributeDescription& Attribute : 
		InGraphicsPipeline.AttributeDescriptions)
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
		InGraphicsPipeline.RasterizerState.bEnableDepthClamp ? VK_TRUE : VK_FALSE,
		InGraphicsPipeline.RasterizerState.bEnableRasterizerDiscard ? VK_TRUE : VK_FALSE,
		VulkanUtil::PolygonModeToVkPolygonMode(InGraphicsPipeline.RasterizerState.PolygonMode),
		VulkanUtil::CullModeToVkCullMode(InGraphicsPipeline.RasterizerState.CullMode),
		VulkanUtil::FrontFaceToVkFrontFace(InGraphicsPipeline.RasterizerState.FrontFace),
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
		InGraphicsPipeline.DepthStencilState.bEnableDepthTest ? VK_TRUE : VK_FALSE,
		InGraphicsPipeline.DepthStencilState.bEnableDepthWrite ? VK_TRUE : VK_FALSE,
		VulkanUtil::ComparisonOpToVkCompareOp(InGraphicsPipeline.DepthStencilState.DepthCompareOp),
		InGraphicsPipeline.DepthStencilState.bDepthBoundsTestEnable ? VK_TRUE : VK_FALSE,
		InGraphicsPipeline.DepthStencilState.bStencilTestEnable ? VK_TRUE : VK_FALSE,
		vk::StencilOpState(
			VulkanUtil::StencilOpToVkStencilOp(InGraphicsPipeline.DepthStencilState.FrontFace.FailOp),
			VulkanUtil::StencilOpToVkStencilOp(InGraphicsPipeline.DepthStencilState.FrontFace.PassOp),
			VulkanUtil::StencilOpToVkStencilOp(InGraphicsPipeline.DepthStencilState.FrontFace.DepthFailOp),
			VulkanUtil::ComparisonOpToVkCompareOp(InGraphicsPipeline.DepthStencilState.FrontFace.CompareOp)),
		vk::StencilOpState(
			VulkanUtil::StencilOpToVkStencilOp(InGraphicsPipeline.DepthStencilState.BackFace.FailOp),
			VulkanUtil::StencilOpToVkStencilOp(InGraphicsPipeline.DepthStencilState.BackFace.PassOp),
			VulkanUtil::StencilOpToVkStencilOp(InGraphicsPipeline.DepthStencilState.BackFace.DepthFailOp),
			VulkanUtil::ComparisonOpToVkCompareOp(InGraphicsPipeline.DepthStencilState.BackFace.CompareOp))
	);
	CreateInfo.setPDepthStencilState(&DepthState);

	/** Color blending */
	std::vector<vk::PipelineColorBlendAttachmentState> ColorAttachments;
	ColorAttachments.reserve(GMaxRenderTargetPerFramebuffer);

	for(const auto& Desc : InGraphicsPipeline.BlendState.BlendDescs)
	{
		ColorAttachments.emplace_back(
			Desc.bEnableBlend ? VK_TRUE : VK_FALSE,
			VulkanUtil::BlendFactorToVkBlendFactor(Desc.SrcColor),
			VulkanUtil::BlendFactorToVkBlendFactor(Desc.DstColor),
			VulkanUtil::BlendOpToVkBlendOp(Desc.ColorOp),
			VulkanUtil::BlendFactorToVkBlendFactor(Desc.SrcAlpha),
			VulkanUtil::BlendFactorToVkBlendFactor(Desc.DstAlpha),
			VulkanUtil::BlendOpToVkBlendOp(Desc.AlphaOp),
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	}

	/**
	 * Color blend state
	 */
	vk::PipelineColorBlendStateCreateInfo ColorBlendState(
		vk::PipelineColorBlendStateCreateFlags(),
		VK_FALSE,
		vk::LogicOp::eCopy,
		static_cast<uint32_t>(ColorAttachments.size()),
		ColorAttachments.data());
	CreateInfo.setPColorBlendState(&ColorBlendState);

	/**
	 * Dynamic states
	 */
	vk::PipelineDynamicStateCreateInfo DynamicState = vk::PipelineDynamicStateCreateInfo(
		vk::PipelineDynamicStateCreateFlags(),
		static_cast<uint32_t>(DynamicStates.size()),
		DynamicStates.data());
	CreateInfo.setPDynamicState(&DynamicState);

	Pipeline = Device.GetDevice().createGraphicsPipelineUnique(vk::PipelineCache(),
		CreateInfo).value;
	if (!Pipeline)
		ZE::Logger::Fatal("Failed to create pipeline");
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
	case EShaderParameterType::Sampler:
		return vk::DescriptorType::eSampler;
	case EShaderParameterType::Texture:
		return vk::DescriptorType::eSampledImage;
	case EShaderParameterType::StorageBuffer:
		return vk::DescriptorType::eStorageBuffer;
	}
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
		return vk::CompareOp::eLessOrEqual;
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