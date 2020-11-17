#pragma once

#include <cmath>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include <numbers>

namespace ze
{

/**
 * From boost's hash_combine
 */
template <class T, class H = std::hash<T>>
ZE_FORCEINLINE void hash_combine(std::size_t& seed, const T& v)
{
	seed ^= H()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace maths
{

/** Degrees to radians */
template<typename T>
	requires std::is_floating_point_v<T>
ZE_FORCEINLINE T radians(const T& in_deg)
{
	return in_deg * (std::numbers::pi / 180);
}

}

}