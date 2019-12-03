#pragma once

class IShader;
class IShaderAttributesManager;

/**
 * Pipeline interface
 */
class IPipeline
{
public:
	virtual ~IPipeline() = default;

	virtual IShaderAttributesManager* GetShaderAttributesManager() const = 0;
};

/**
 * Pipeline create infos
 */
struct SGraphicsPipelineInfos
{
	IShader* VertexShader;
	IShader* FragmentShader;
	SVertexInputBindingDescription BindingDescription;
	std::vector<SVertexInputAttributeDescription> AttributeDescriptions;
	std::vector<SShaderAttribute> ShaderAttributes;

	SGraphicsPipelineInfos(IShader* InVertexShader,
		IShader* InFragmentShader,
		const SVertexInputBindingDescription& InBindingDescription,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions,
		const std::vector<SShaderAttribute>& InShaderAttributes) : VertexShader(InVertexShader),
		FragmentShader(InFragmentShader), BindingDescription(InBindingDescription),
		AttributeDescriptions(InAttributeDescriptions), ShaderAttributes(InShaderAttributes) {}
};

/**
 * Graphics pipeline
 */
class IGraphicsPipeline : public IPipeline
{
public:
	IGraphicsPipeline(const SGraphicsPipelineInfos& InInfos) {}
	
	virtual ~IGraphicsPipeline() = default;
};