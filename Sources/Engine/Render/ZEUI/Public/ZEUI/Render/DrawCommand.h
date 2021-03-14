#pragma once

#include "EngineCore.h"
#include "Rendering.h"
#include "Gfx/Effect/Effect.h"

namespace ze::ui
{

struct DrawCommand;

/**
 * Base class for draw commands primitives
 * Primitives draw commands classes are used to build the geometry of a command
 * Widgets can submit multiple primitives
 */
struct DrawCommandPrimitive
{
	struct Binding
	{
		gfx::DescriptorType type;
		uint32_t set;
		uint32_t binding;
		gfx::DeviceResourceHandle resource;

		Binding(const gfx::DescriptorType& in_type,
			const uint32_t in_set,
			const uint32_t in_binding,
			const gfx::DeviceResourceHandle& in_resource) :
			type(in_type), set(in_set), binding(in_binding), resource(in_resource) {}

		ZE_FORCEINLINE bool operator==(const Binding& other) const
		{
			return type == other.type &&
				set == other.set &&
				binding == other.binding &&
				resource == other.resource;
		}
	};

	DrawCommandPrimitive() : effect(nullptr) {}
	virtual ~DrawCommandPrimitive() = default;
	virtual void build(const DrawCommand& commmand) = 0;

	ZE_FORCEINLINE gfx::EffectPermPtr get_effect() const { return effect; }
	ZE_FORCEINLINE std::vector<Binding> get_bindings() const { return bindings; }
	ZE_FORCEINLINE const std::vector<Vertex>& get_vertices() const { return vertices; }
	ZE_FORCEINLINE const std::vector<uint32_t>& get_indices() const { return indices; }
private:
	/*
	 * Build primitives geometry and effect/bindings
	 */
protected:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Binding> bindings;
	gfx::EffectPermPtr effect;
};

/**
 * A single draw command
 * A widget may be composed of multiple drawcommands that are merged
 */
struct DrawCommand
{
	std::unique_ptr<DrawCommandPrimitive> primitive;
	maths::Vector2f position;
	maths::Vector2f size;

	DrawCommand(OwnerPtr<DrawCommandPrimitive> in_primitive,
		const maths::Vector2f in_position,
		const maths::Vector2f& in_size) 
		: primitive(in_primitive), position(in_position), size(in_size) {}

	bool operator==(const DrawCommand& other) const
	{
		return primitive == other.primitive;
	}
};

}