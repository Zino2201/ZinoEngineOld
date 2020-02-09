#include "VulkanPipeline.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderSystem.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"
#include "VulkanShaderAttributesManager.h"

/** --- CVulkanPipeline --- */
CVulkanPipeline::CVulkanPipeline(CVulkanDevice* InDevice,
	const std::vector<SShaderAttribute>& InShaderAttributes) : CVulkanDeviceResource(InDevice) 
{
	ShaderAttributes = InShaderAttributes;

	std::vector<vk::DescriptorSetLayout> SetLayouts;

	/** Create descriptor set layout and pool */
	{
		for (const SShaderAttribute& Attribute : InShaderAttributes)
		{
			AttributeMap[Attribute.Frequency].push_back(Attribute);
		}

		for(const auto& [Frequency, Attributes] : AttributeMap)
		{
			std::vector<vk::DescriptorSetLayoutBinding> SetLayoutBindings;

			for(const SShaderAttribute& Attribute : Attributes)
			{
				SetLayoutBindings.emplace_back(
					Attribute.Binding,
					VulkanUtil::ShaderAttributeTypeToVkDescriptorType(Attribute.Type),
					Attribute.Count,
					VulkanUtil::ShaderStageFlagsToVkShaderStageFlags(Attribute.StageFlags));

				PoolSizeMap[Attribute.Frequency].emplace_back(VulkanUtil::ShaderAttributeTypeToVkDescriptorType(Attribute.Type),
					static_cast<uint32_t>(g_VulkanRenderSystem->GetSwapChain()->GetImageViews().size()));
			}

			DescriptorSetLayoutMap.insert(std::make_pair(Frequency, 
				Device->GetDevice().createDescriptorSetLayoutUnique(
					vk::DescriptorSetLayoutCreateInfo(
						vk::DescriptorSetLayoutCreateFlags(),
						static_cast<uint32_t>(SetLayoutBindings.size()),
						SetLayoutBindings.data())).value));

			SetLayouts.emplace_back(*DescriptorSetLayoutMap[Frequency]);
		}
	}

	/** Create pipeline layout */
	{
		PipelineLayout = std::make_unique<CVulkanPipelineLayout>(Device, SetLayouts);
	}

	Create();
}

CVulkanPipeline::~CVulkanPipeline() {}

void CVulkanPipeline::Create()
{
}

std::shared_ptr<IShaderAttributesManager> CVulkanPipeline::CreateShaderAttributesManager(EShaderAttributeFrequency InFrequency) const
{
	SShaderAttributesManagerInfo Infos;
	Infos.Pipeline = const_cast<CVulkanPipeline*>(this);
	Infos.Frequency = InFrequency;

	return std::make_shared<CVulkanShaderAttributesManager>(
		Infos);
}

/** --- CVulkanPipeline --- */

CVulkanGraphicsPipeline::CVulkanGraphicsPipeline(CVulkanDevice* InDevice, 
	const SRenderSystemGraphicsPipelineInfos& InInfos)
	: IRenderSystemGraphicsPipeline(InInfos), CVulkanPipeline(InDevice, InInfos.ShaderAttributes)
{
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
	vk::VertexInputBindingDescription InputAttribute(
		InInfos.BindingDescription.Binding,
		InInfos.BindingDescription.Stride,
		VulkanUtil::VertexInputRateToVkVertexInputRate(InInfos.BindingDescription.InputRate));

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
		1,
		&InputAttribute,
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
		VK_FALSE,
		VK_FALSE,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eCounterClockwise,
		VK_FALSE,
		0.f,
		0.f,
		0.f,
		1.f);

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
		VK_TRUE,
		VK_TRUE,
		vk::CompareOp::eLess,
		VK_FALSE);

	/** Color blending */
	vk::PipelineColorBlendAttachmentState ColorBlendAttachment(
		VK_FALSE,
		vk::BlendFactor::eOne,
		vk::BlendFactor::eZero,
		vk::BlendOp::eAdd,
		vk::BlendFactor::eOne,
		vk::BlendFactor::eZero,	
		vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo ColorBlendState(
		vk::PipelineColorBlendStateCreateFlags(),
		VK_FALSE,
		vk::LogicOp::eCopy,
		1,
		&ColorBlendAttachment);

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
		nullptr,
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