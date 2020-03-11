#pragma once

#include "EngineCore.h"

struct STransform
{
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;

	STransform() : Position(), Rotation(), Scale() {}
	STransform(const glm::dvec3& InPosition) : Position(InPosition) {}
};

/*
 * A 2D rectangle
 */
struct SRect2D
{
	glm::vec2 Position;
	glm::vec2 Size;
};