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
 */
struct DrawCommandPrimitive
{
	virtual ~DrawCommandPrimitive() = default;

	virtual std::pair<std::vector<Vertex>, std::vector<uint32_t>> get_geometry(const DrawCommand& commmand) = 0;
	virtual gfx::EffectPermPtr get_effect() const { return nullptr; }
	virtual std::vector<gfx::ResourceHandle> get_descriptor_sets() const { return {}; }
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