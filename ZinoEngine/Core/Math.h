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