#pragma once

class IShader;

/**
 * Pipeline interface
 */
class IPipeline
{
public:
	virtual ~IPipeline() = default;
};

struct SVertexInputBindingDescription
{
	uint32_t Binding;
	uint32_t Stride;
	EVertexInputRate InputRate;

	SVertexInputBindingDescription(const uint32_t& InBinding,
		const uint32_t& InStride, const EVertexInputRate& InInputRate) :
		Binding(InBinding), Stride(InStride), InputRate(InInputRate) {}
};

struct SVertexInputAttributeDescription
{
	uint32_t Binding;
	uint32_t Location;
	EFormat Format;
	uint32_t Offset;

	SVertexInputAttributeDescription(const uint32_t& InBinding,
		const uint32_t& InLocation, const EFormat& InFormat, const uint32_t& InOffset) :
		Binding(InBinding), Location(InLocation), Format(InFormat), Offset(InOffset) {}
};

/**
 * Graphics pipeline
 */
class IGraphicsPipeline : public IPipeline
{
public:
	IGraphicsPipeline(IShader* InVertexShader,
		IShader* InFragmentShader,
		const SVertexInputBindingDescription& InBindingDescription,
		const std::vector<SVertexInputAttributeDescription>& InAttributeDescriptions) {}
	
	virtual ~IGraphicsPipeline() = default;
};