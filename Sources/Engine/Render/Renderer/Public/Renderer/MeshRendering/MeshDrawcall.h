#pragma once

#include "Render/RenderCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/GraphicsPipeline.h"
#include "Containers/CoherentArray.h"
#include "Shader/ShaderCore.h"

namespace ze::renderer
{

/**
 * A drawcall shader binding
 */
struct SMeshDrawcallShaderBinding
{
	uint32_t Set;
	uint32_t Binding;
	ShaderParameterType ParameterType;
	union
	{
		CRSBuffer* Buffer;
		CRSTexture* Texture;
		CRSSampler* Sampler;
	};

	SMeshDrawcallShaderBinding() : Set(0), Binding(0),
		ParameterType(ShaderParameterType::UniformBuffer),
		Buffer(nullptr) {}

	SMeshDrawcallShaderBinding(const uint32_t& InSet,
		const uint32_t& InBinding,
		const ShaderParameterType& InParameterType,
		CRSBuffer* InBuffer) : Set(InSet), Binding(InBinding),
		ParameterType(InParameterType),
		Buffer(InBuffer) {}
};

/**
 * A drawcall used to render a mesh
 * Can represent a simple draw, a indexed draw, or a instanced draw
 */
struct SMeshDrawcall
{
	SRSGraphicsPipeline Pipeline;
	std::array<SMeshDrawcallShaderBinding, GMaxBindings * GMaxDescriptorSets> Bindings;
	CRSBuffer* VertexBuffer;
	CRSBuffer* IndexBuffer;
	EIndexFormat IndexFormat;
	union
	{
		uint32_t VertexCount;
		uint32_t IndexCount;
	};
	uint32_t InstanceCount;
	union
	{
		uint32_t FirstVertex;
		uint32_t FirstIndex;
	};
	uint32_t FirstInstance;
	
	SMeshDrawcall() : VertexBuffer(nullptr), IndexBuffer(nullptr), IndexFormat(EIndexFormat::Uint16),
		VertexCount(0), InstanceCount(1), FirstVertex(0),
		FirstInstance(0) {}

	SMeshDrawcall(const SRSGraphicsPipeline& InPipeline,
		CRSBuffer* InVertexBuffer,
		CRSBuffer* InIndexBuffer,
		const EIndexFormat& InIndexFormat,
		const uint32_t& InVertexOrIndexCount,
		const uint32_t& InInstanceCount,
		const uint32_t& InFirstVertexOrIndex,
		const uint32_t& InFirstInstance) :
		Pipeline(InPipeline), VertexBuffer(InVertexBuffer),
		IndexBuffer(InIndexBuffer), IndexFormat(InIndexFormat), 
		VertexCount(InVertexOrIndexCount),
		InstanceCount(InInstanceCount),
		FirstVertex(InFirstVertexOrIndex), FirstInstance(InFirstInstance) {}
};

using TDrawcallList = TCoherentArray<SMeshDrawcall>;
using TTransientDrawcallRefList = std::vector<const SMeshDrawcall*>;

}