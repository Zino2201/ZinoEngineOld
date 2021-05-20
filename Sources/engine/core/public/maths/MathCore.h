#pragma once

#include <cmath>
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