#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/ext/matrix_double4x4.hpp>

namespace ZE::Math
{

/*
 * A 2D rectangle
 */
struct SRect2D
{
	glm::vec2 Position;
	glm::vec2 Size;
};

} /* namespace ZE::Math */

/**
 * From boost's hash_combine
 */
template <class T, class H = std::hash<T>>
inline void HashCombine(std::size_t& Seed, const T& V)
{
	H Hasher;
	Seed ^= Hasher(V) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
}