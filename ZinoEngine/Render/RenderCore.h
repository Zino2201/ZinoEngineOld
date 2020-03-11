#pragma once

#include "Core/EngineCore.h"
#include "Core/RenderThread.h"

/**
 * Shader stages
 */
enum class EShaderStage
{
	Vertex = 1 << 0,
	Fragment = 1 << 1
};
DECLARE_FLAG_ENUM(EShaderStage)

/**
 * Shader parameter type
 */
enum class EShaderParameterType
{
	UniformBuffer,
	CombinedImageSampler,
	StorageBuffer
};

/**
 * A member of a shader parameter
 */
struct SShaderParameterMember
{
	/** Member name */
	std::string Name;
	uint64_t Size;
	uint64_t Offset;

	/** If member is a struct, members */
	std::vector<SShaderParameterMember> Members;

	static const SShaderParameterMember& GetNull()
	{
		static SShaderParameterMember Null;
		return Null;
	}

	bool operator==(const SShaderParameterMember& InOther) const
	{
		return InOther.Name == Name;
	}
};

/**
 * A shader parameter
 */
struct SShaderParameter
{
	/** Parameter name */
	std::string Name;

	/** Parameter type */
	EShaderParameterType Type;

	/** Parameter set (used only with render systems that support descriptor sets like Vulkan) */
	uint32_t Set;

	/** Parameter binding point */
	uint32_t Binding;

	/** Parameter size */
	uint64_t Size;

	/** Parameter count (> 1 if an array) */
	uint32_t Count;

	/** Shader stage flags */
	EShaderStageFlags StageFlags;

	/** Members if any */
	std::vector<SShaderParameterMember> Members;

	static const SShaderParameter& GetNull()
	{
		static SShaderParameter Null;
		return Null;
	}

	const SShaderParameterMember& GetMember(const std::string& InName) const
	{
		for(const auto& Member : Members)
		{
			if(Member.Name == InName)
				return Member;
		}

		return SShaderParameterMember::GetNull();
	}
};

namespace std
{
	template<> struct less<SShaderParameter>
	{
		bool operator()(const SShaderParameter& LHS, const SShaderParameter& RHS) const
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
	R32Uint, /** uint32_t */
	R8G8B8A8UNorm, /** rgb 255*/
	R32G32Sfloat, /** vec2*/
	R32G32B32Sfloat, /** vec3 */
	R32G32B32A32Sfloat,	/** vec4 */
	R64Uint	/** uint64 */
};

/** Index format */
enum class EIndexFormat
{
	Uint16,
	Uint32
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

	static std::vector<SVertexInputBindingDescription> GetBindingDescriptions()
	{
		return 
		{ 
			SVertexInputBindingDescription(0, sizeof(SVertex), EVertexInputRate::Vertex),
			SVertexInputBindingDescription(1, sizeof(uint32_t), EVertexInputRate::Instance),
		};
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
				offsetof(SVertex, Normal)),
			SVertexInputAttributeDescription(1, 4, EFormat::R32Uint, 0)
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
	std::string Name;
	ERenderSystemType Type;
	ERenderSystemShaderFormat Format;
};

/**
 * An viewport
 */
struct SViewport
{
	SRect2D Rect;
	float MinDepth;
	float MaxDepth;
};