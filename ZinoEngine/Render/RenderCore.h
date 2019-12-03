#pragma once

#include "Core/EngineCore.h"

/**
 * Shader stages
 */
enum class EShaderStage
{
	Vertex,
	Fragment
};
DECLARE_FLAG_ENUM(EShaderStage)

/** Shader attribute type */
enum class EShaderAttributeType
{
	/** A uniform buffer that keep its size */
	UniformBufferStatic
};

/**
 * Shader attribute
 */
struct SShaderAttribute
{
	/** Name */
	std::string Name;

	/** Binding */
	uint32_t Binding;

	/** Attribute type */
	EShaderAttributeType Type;

	/** Stages */
	EShaderStageFlags StageFlags;

	/** Count, should be 1 if not an array */
	uint32_t Count;

	SShaderAttribute(
		const std::string& InName,
		const uint32_t& InBinding,
		const EShaderAttributeType& InType,
		const EShaderStageFlags& InStageFlags,
		const uint32_t& InCount = 1) : Name(InName), Binding(InBinding), Type(InType),
		StageFlags(InStageFlags), Count(InCount) {}
};

/** Vertex input rate */
enum class EVertexInputRate
{
	Vertex,
	Instance
};

/** Format */
enum class EFormat
{
	R32G32Sfloat,
	R32G32B32Sfloat,
};

/** Index format */
enum class EIndexFormat
{
	Uint16,
	Uint32
};

/**
 * Buffer usage
 */
enum class EBufferUsage
{
	VertexBuffer = 1 << 0,
	IndexBuffer	= 1 << 1,
	TransferSrc = 1 << 2,
	TransferDst = 1 << 3,
	UniformBuffer = 1 << 4
};
DECLARE_FLAG_ENUM(EBufferUsage)

enum class EBufferMemoryUsage
{
	CpuOnly,
	GpuOnly,
	CpuToGpu,
	GpuToCpu
};

/**
 * Max frames in flight
 */
const uint32_t g_MaxFramesInFlight = 2;

/** Vertex inputs */
/**
 * Vertex binding description
 */
struct SVertexInputBindingDescription
{
	uint32_t Binding;
	uint32_t Stride;
	EVertexInputRate InputRate;

	SVertexInputBindingDescription(const uint32_t& InBinding,
		const uint32_t& InStride, const EVertexInputRate& InInputRate) :
		Binding(InBinding), Stride(InStride), InputRate(InInputRate) {}
};

/**
 * Vertex input description
 */
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

/** Vertex structure */
struct SVertex
{
	glm::vec2 Position;
	glm::vec3 Color;

	static SVertexInputBindingDescription GetBindingDescription()
	{
		return SVertexInputBindingDescription(0, sizeof(SVertex), EVertexInputRate::Vertex);
	}

	static std::vector<SVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		return
		{
			SVertexInputAttributeDescription(0, 0, EFormat::R32G32Sfloat,
				offsetof(SVertex, Position)),
			SVertexInputAttributeDescription(0, 1, EFormat::R32G32B32Sfloat,
				offsetof(SVertex, Color)),
		};
	}
};
