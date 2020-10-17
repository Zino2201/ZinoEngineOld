#pragma once

#include "MathCore.h"

namespace ze::maths
{

/**
 * Base template class for a 2D vector
 */
template<typename T>
	requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
struct Vector2
{
	T x;
	T y;

	Vector2() : x(0), y(0) {}
	Vector2(const T& in_val) : x(in_val), y(in_val) {}
	Vector2(const T& in_x, const T& in_y) : x(in_x), y(in_y) {}

	bool operator==(const Vector2& other) const
	{
		return x == other.x 
			&& y == other.y;
	}

	bool operator!=(const Vector2& other) const
	{
		return x != other.x ||
			y != other.y;
	}
};

using Vector2f = Vector2<float>;

/**
 * Basic template class for a 3D vector
 */
template<typename T>
	requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
struct Vector3
{
	T x;
	T y;
	T z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(const T& in_val) : x(in_val), y(in_val), z(in_val) {}
	Vector3(const T& in_x, const T& in_y, const T& in_z) : x(in_x), y(in_y), z(in_z) {}
	Vector3(const Vector2<T>& in_vec2, const T& in_z) : x(in_vec2.x), y(in_vec2.y), z(in_z) {}

	bool operator==(const Vector3& other) const
	{
		return x == other.x 
			&& y == other.y 
			&& z == other.z;
	}

	bool operator!=(const Vector3& other) const
	{
		return x != other.x ||
			y != other.y ||
			z != other.z;
	}
};

/**
 * A 3d vector (double-precision)
 */
struct Vector3d : public Vector3<double>
{
	Vector3d& operator=(const glm::vec3& vec)
	{
		x = vec.x;
		y = vec.x;
		z = vec.z;

		return *this;
	}
};

/**
 * A 3d vector (single-precision)
 */
using Vector3f = Vector3<float>;

} /* namespace ZE::Math */

namespace std
{
	template<typename T> 
	struct hash<ze::maths::Vector2<T>>
	{
		std::size_t operator()(const ze::maths::Vector2<T> in_vec) const noexcept
		{
			size_t hash = 0;
			ze::hash_combine(hash, in_vec.x);
			ze::hash_combine(hash, in_vec.y);
			return hash;
		}
	};

	template<typename T> 
	struct hash<ze::maths::Vector3<T>>
	{
		std::size_t operator()(const ze::maths::Vector3<T> in_vec) const noexcept
		{
			size_t hash = 0;
			ze::hash_combine(hash, in_vec.x);
			ze::hash_combine(hash, in_vec.y);
			ze::hash_combine(hash, in_vec.z);
			return hash;
		}
	};
}