#pragma once

#include "EngineCore.h"
#include "Rendering.h"

namespace ze::ui
{

class DrawCommand;

/**
 * Base class for draw commands primitives
 * Primitives draw commands classes are used to build the geometry of a command
 */
struct DrawCommandPrimitive
{
	virtual std::pair<std::vector<Vertex>, std::vector<uint32_t>> get_geometry(const DrawCommand& commmand) = 0;
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
};

}