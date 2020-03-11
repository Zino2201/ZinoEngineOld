#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"

/** --- CVulkanPipeline --- */
CVulkanPipeline::CVulkanPipeline(CVulkanDevice* InDevice,
	const std::vector<SShaderParameter>& InShaderParameters) : CVulkanDeviceResource(InDevice),
	ShaderParameters(InShaderParameters)
{
	// TODO: Hashing for pipeline layouts
	//	for now every pipeline create its own pipeline layout

	/** Shader parameters */
	//{
	//	/** Added parameters map (name, index) */
	//	std::map<std::string, size_t> AddedParameters;

	//	for (const auto& [Stage, Shader] : ShaderMap.GetMap())
	//	{
	//		for (const auto& Parameter : Shader->GetParameters())
	//		{
	//			if (AddedParameters.count(Parameter.Name))
	//			{
	//				ShaderParameters[AddedParameters[Parameter.Name]].StageFlags |=
	//					Stage;
	//			}
	//			else
	//			{
	//				ShaderParameters.push_back(Parameter);
	//				AddedParameters[Parameter.Name] = ShaderParameters.size() - 1;
	//			}
	//		}
	//	}
	//}

	/** Create set layouts and bindings per sets  */
	for(const auto& Parameter : ShaderParameters)
	{
		SetLayoutBindings[Parameter.Set].emplace_back(
			Parameter.Binding,
			VulkanUtil::ShaderParameterTypeToVkDescriptorType(Parameter.Type),
			Parameter.Count,
			VulkanUtil::ShaderStageFlagsToVkShaderStageFlags(Parameter.StageFlags));
	}

	/** Create pipeline layout */
	{
		SVulkanPipelineLayoutInfos Infos;
		Infos.SetLayoutBindings = SetLayoutBindings;
		PipelineLayout = std::make_unique<CVulkanPipelineLayout>(Device, Infos);
	}

	Create();
}

CVulkanPipeline::~CVulkanPipeline() {}

void CVulkanPipeline::Create()
{

}

/** --- CVulkanPipeline --- */

CVulkanGraphicsPipeline::CVulkanGraphicsPipeline(CVulkanDevice* InDevice, 
	const SRenderSystemGraphicsPipelineInfos& InInfos)
	: IRenderSystemGraphicsPipeline(InInfos), CVulkanPipeline(InDevice, InInfos.ShaderParameters)
{
	std::array<vk::DynamicState, 2> DynamicStates =
	{
		vk::DynamicState::eScissor,
		vk::DynamicState::eViewport
	};

	/** Create stages */
	std::vector<CVulkanShader*> Shaders =
	{
		static_cast<CVulkanShader*>(InInfos.VertexShader),
		static_cast<CVulkanShader*>(InInfos.FragmentShader)
	};

	std::vector<vk::PipelineShaderStageCreateInfo> Stages(Shaders.size());

	for (int i = 0; i < Shaders.size(); ++i)
	{
		CVulkanShader* Shader = Shaders[i];

		Stages[i] = vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags(),
			Shader->GetStageFlag(),
			Shader->GetShaderModule(),
			Shader->GetMain());
	}

	/** Input attributes */
	std::vector<vk::VertexInputBindingDescription> InputAttributes;
	for(const auto& BindingDescription : InInfos.BindingDescriptions)
	{
		InputAttributes.emplace_back(
			BindingDescription.Binding,
			BindingDescription.Stride,
			VulkanUtil::VertexInputRateToVkVertexInputRate(BindingDescription.InputRate));
	}

	/** Vertex attributes */
	std::vector<vk::VertexInputAttributeDescription> Attributes;
	for(const SVertexInputAttributeDescription& Attribute : InInfos.AttributeDescriptions)
	{
		Attributes.emplace_back(
			Attribute.Location,
			Attribute.Binding,
			VulkanUtil::FormatToVkFormat(Attribute.Format),
			Attribute.Offset);
	}

	/** Vertex input infos */
	vk::PipelineVertexInputStateCreateInfo VertexInputInfos(
		vk::PipelineVertexInputStateCreateFlags(),
		static_cast<uint32_t>(InputAttributes.size()),
		InputAttributes.data(),
		static_cast<uint32_t>(Attributes.size()),
		Attributes.data());

	/** Input assembly */
	vk::PipelineInputAssemblyStateCreateInfo InputAssembly(
		vk::PipelineInputAssemblyStateCreateFlags(),
		vk::PrimitiveTopology::eTriangleList,
		VK_FALSE);

	/** Viewport */
	vk::Viewport Viewport(
		0,
		0,
		static_cast<float>(g_VulkanRenderSystem->GetSwapChain()->GetExtent().width),
		static_cast<float>(g_VulkanRenderSystem->GetSwapChain()->GetExtent().height),
		0.f,
		1.f);

	vk::Rect2D Scissor(
		vk::Offset2D(0, 0),
		g_VulkanRenderSystem->GetSwapChain()->GetExtent());

	vk::PipelineViewportStateCreateInfo ViewportState(
		vk::PipelineViewportStateCreateFlags(),
		1,
		&Viewport,
		1,
		&Scissor);

	/** Rasterizer */
	vk::PipelineRasterizationStateCreateInfo RasterizerState(
		vk::PipelineRasterizationStateCreateFlags(),
		InInfos.RasterizerState.bDepthClampEnable ? VK_TRUE : VK_FALSE,
		InInfos.RasterizerState.bRasterizerDiscardEnable ? VK_TRUE : VK_FALSE,
		VulkanUtil::PolygonModeToVkPolygonMode(InInfos.RasterizerState.PolygonMode),
		VulkanUtil::CullModeToVkCullMode(InInfos.RasterizerState.CullMode), 
		VulkanUtil::FrontFaceToVkFrontFace(InInfos.RasterizerState.FrontFace),
		VK_FALSE,
		0.f,
		0.f,
		0.f,
		1.f);

	//vk::DynamicState::

	/** Multisampling */
	vk::PipelineMultisampleStateCreateInfo MultisamplingState(
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1,
		VK_FALSE,
		1.f,
		nullptr,
		VK_FALSE,
		VK_FALSE);

	vk::PipelineDepthStencilStateCreateInfo DepthState(
		vk::PipelineDepthStencilStateCreateFlags(),
		InInfos.DepthStencilState.bDepthTestEnable ? VK_TRUE : VK_FALSE,
		InInfos.DepthStencilState.bDepthWriteEnable ? VK_TRUE : VK_FALSE,
		VulkanUtil::ComparisonOpToVkCompareOp(InInfos.DepthStencilState.DepthCompareOp),
		InInfos.DepthStencilState.bDepthBoundsTestEnable ? VK_TRUE : VK_FALSE,
		InInfos.DepthStencilState.bStencilTestEnable ? VK_TRUE : VK_FALSE,
		vk::StencilOpState(
			VulkanUtil::StencilOpToVkStencilOp(InInfos.DepthStencilState.FrontFace.FailOp),
			VulkanUtil::StencilOpToVkStencilOp(InInfos.DepthStencilState.FrontFace.PassOp),
			VulkanUtil::StencilOpToVkStencilOp(InInfos.DepthStencilState.FrontFace.DepthFailOp),
			VulkanUtil::ComparisonOpToVkCompareOp(InInfos.DepthStencilState.FrontFace.CompareOp)),
		vk::StencilOpState(
			VulkanUtil::StencilOpToVkStencilOp(InInfos.DepthStencilState.BackFace.FailOp),
			VulkanUtil::StencilOpToVkStencilOp(InInfos.DepthStencilState.BackFace.PassOp),
			VulkanUtil::StencilOpToVkStencilOp(InInfos.DepthStencilState.BackFace.DepthFailOp),
			VulkanUtil::ComparisonOpToVkCompareOp(InInfos.DepthStencilState.BackFace.CompareOp))
		);

	/** Color blending */
	vk::PipelineColorBlendAttachmentState ColorBlendAttachment(
		InInfos.BlendState.bEnableBlend ? VK_TRUE : VK_FALSE,
		VulkanUtil::BlendFactorToVkBlendFactor(InInfos.BlendState.SrcColorFactor),
		VulkanUtil::BlendFactorToVkBlendFactor(InInfos.BlendState.DestColorFactor),
		VulkanUtil::BlendOpToVkBlendOp(InInfos.BlendState.ColorBlendOp),
		VulkanUtil::BlendFactorToVkBlendFactor(InInfos.BlendState.SrcAlphaFactor),
		VulkanUtil::BlendFactorToVkBlendFactor(InInfos.BlendState.DestAlphaFactor),
		VulkanUtil::BlendOpToVkBlendOp(InInfos.BlendState.AlphaBlendOp),
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo ColorBlendState(
		vk::PipelineColorBlendStateCreateFlags(),
		VK_FALSE,
		vk::LogicOp::eCopy,
		1,
		&ColorBlendAttachment);

	vk::PipelineDynamicStateCreateInfo DynamicState = vk::PipelineDynamicStateCreateInfo(
		vk::PipelineDynamicStateCreateFlags(),
		DynamicStates.size(),
		DynamicStates.data());

	/** Create pipeline */
	vk::GraphicsPipelineCreateInfo CreateInfos(
		vk::PipelineCreateFlags(),
		static_cast<uint32_t>(Stages.size()),
		Stages.data(),
		&VertexInputInfos,
		&InputAssembly,
		nullptr,
		&ViewportState,
		&RasterizerState,
		&MultisamplingState,
		&DepthState,
		&ColorBlendState,
		&DynamicState,
		PipelineLayout->GetPipelineLayout(),
		g_VulkanRenderSystem->GetRenderPass(),
		0,
		vk::Pipeline(),
		-1);

	Pipeline = Device->GetDevice().createGraphicsPipelineUnique(vk::PipelineCache(),
		CreateInfos).value;
	if(!Pipeline)
		LOG(ELogSeverity::Fatal, "Failed to create pipeline")
}

CVulkanGraphicsPipeline::~CVulkanGraphicsPipeline() {}