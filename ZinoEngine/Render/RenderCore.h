#pragma once

#include "Core/EngineCore.h"

/**
 * Shader parameter type
 */
enum class EShaderParameterType
{
	UniformBuffer,
	CombinedImageSampler
};

/**
 * A shader parameter
 */
struct SShaderParameter
{
	std::string Name;
	EShaderParameterType Type;
	uint32_t Set;
	uint32_t Binding;
	uint64_t Size;
	uint32_t Count;
};

/**
 * Shader stages
 */
enum class EShaderStage
{
	Vertex,
	Fragment
};
DECLARE_FLAG_ENUM(EShaderStage)

/**
 * Shader related elements
 */
enum class EShaderParameterType__
{
	/** Uniform buffer */
	UniformBuffer,

	/** Sampler */
	Sampler,

	/** Vulkan only */
	CombinedImageSampler
};

/** Shader attribute type */
enum class EShaderAttributeType
{
	/** A uniform buffer that keep its size */
	UniformBufferStatic,

	/** Combined image sampler */
	CombinedImageSampler
};

enum class EShaderAttributeFrequency : uint8_t
{
	PerMaterial,
	PerInstance
};

enum class EShaderAttributeMeta
{
	None = -1,

	/** Vulkan push constant */
	VulkanPushConstant = 1 << 0
};
DECLARE_FLAG_ENUM(EShaderAttributeMeta)


/**
 * Member of a attribute
 */
struct SShaderAttributeMember
{
	std::string Name;
	uint64_t Size;
	uint64_t Offset;

	SShaderAttributeMember(const std::string& InName,
		const uint64_t& InSize, const uint64_t& InOffset) : Name(InName),
		Size(InSize), Offset(InOffset) {}
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

	/** Size */
	uint64_t Size;

	/** Attribute type */
	EShaderAttributeType Type;

	/** Attribute frequency */
	EShaderAttributeFrequency Frequency;

	/** Stages */
	EShaderStageFlags StageFlags;

	/** Count, should be 1 if not an array */
	uint32_t Count;

	/** Meta */
	EShaderAttributeMeta Meta;

	/** Members */
	std::vector<SShaderAttributeMember> Members;

	SShaderAttribute(
		const std::string& InName,
		const uint32_t& InBinding,
		const uint64_t& InSize,
		const EShaderAttributeType& InType,
		const EShaderAttributeFrequency& InFrequency,
		const EShaderStageFlags& InStageFlags,
		const uint32_t& InCount = 1,
		const EShaderAttributeMeta& InMeta = EShaderAttributeMeta::None,
		const std::vector<SShaderAttributeMember>& InMembers = {}) : 
		Name(InName), Binding(InBinding), Size(InSize), Type(InType),
		Frequency(InFrequency),
		StageFlags(InStageFlags), Count(InCount), Meta(InMeta),
		Members(InMembers) {}
};

namespace std
{
	template<> struct less<SShaderAttribute>
	{
		bool operator()(const SShaderAttribute& LHS, const SShaderAttribute& RHS) const
		{
			return LHS.Binding < RHS.Binding;
		}
	};
}

/** Vertex input rate */
enum class EVertexInputRate
{
	Vertex,
	Instance
};

/** Format */
enum class EFormat
{
	D32Sfloat,
	D32SfloatS8Uint,
	D24UnormS8Uint,
	R8G8B8A8UNorm,
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

/** Textures */
enum class ETextureType
{
	Texture2D,
};

enum class ETextureViewType
{
	ShaderResource,
	DepthStencil,
};

enum class ETextureUsage
{
	TransferSrc = 1 << 0,
	TransferDst = 1 << 1,
	Sampled = 1 << 2,
	DepthStencil = 1 << 3
};
DECLARE_FLAG_ENUM(ETextureUsage)

enum class ETextureMemoryUsage
{
	CpuOnly,
	GpuOnly,
	CpuToGpu,
	GpuToCpu
};

/** Samplers */
enum class ESamplerFilter
{
	Linear,
	Nearest
};

enum class ESamplerAddressMode
{
	Repeat,
	Mirror,
	Clamp,
	Border,
};

/** Other */
enum class EComparisonOp
{
	Never,
	Always
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
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;
	glm::vec3 Normal;

	SVertex() {}
	SVertex(const glm::vec3& InPosition) : Position(InPosition) {}

	static SVertexInputBindingDescription GetBindingDescription()
	{
		return SVertexInputBindingDescription(0, sizeof(SVertex), EVertexInputRate::Vertex);
	}

	static std::vector<SVertexInputAttributeDescription> GetAttributeDescriptions()
	{
		return
		{
			SVertexInputAttributeDescription(0, 0, EFormat::R32G32B32Sfloat,
				offsetof(SVertex, Position)),
			SVertexInputAttributeDescription(0, 1, EFormat::R32G32B32Sfloat,
				offsetof(SVertex, Color)),
			SVertexInputAttributeDescription(0, 2, EFormat::R32G32Sfloat,
				offsetof(SVertex, TexCoord)),
			SVertexInputAttributeDescription(0, 3, EFormat::R32G32B32Sfloat,
				offsetof(SVertex, Normal))
		};
	}

	bool operator==(const SVertex& InOther) const
	{
		return Position == InOther.Position 
			&& Color == InOther.Color 
			&& TexCoord == InOther.TexCoord
			&& Normal == InOther.Normal;
	}
};

namespace std 
{
	template<> struct hash<SVertex> 
	{
		size_t operator()(const SVertex& InVertex) const
		{
			return ((hash<glm::vec3>()(InVertex.Position) ^
				(hash<glm::vec3>()(InVertex.Color) << 1)) >> 1) ^
				(hash<glm::vec2>()(InVertex.TexCoord) << 1) ^
				(hash<glm::vec3>()(InVertex.Normal) << 1);
		}
	};
}

/**
 * Render System Type
 */
enum class ERenderSystemType
{
	Unknown,
	Vulkan,
	GL3
};

/**
 * Render system shader format
 */
enum class ERenderSystemShaderFormat
{
	SpirV,
	HLSL,
	GLSL
};

struct SRenderSystemDetails
{
	ERenderSystemType Type;
	ERenderSystemShaderFormat Format;
};