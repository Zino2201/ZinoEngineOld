#pragma once

#include "MathCore.h"
#include <array>

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

	template<typename ArchiveType>
    void serialize(ArchiveType& in_archive)
    {
		in_archive <=> x;
		in_archive <=> y;
	}
	
	ZE_FORCEINLINE Vector2& operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	ZE_FORCEINLINE Vector2& operator+=(const T& other)
	{
		x += other;
		y += other;
		return *this;
	}

	ZE_FORCEINLINE Vector2& operator*=(const Vector2& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	ZE_FORCEINLINE Vector2& operator*=(const T& other)
	{
		x *= other;
		y *= other;
		return *this;
	}

	ZE_FORCEINLINE Vector2& operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}


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

template<typename T>
ZE_FORCEINLINE Vector2<T> operator+(const Vector2<T>& a, const Vector2<T>& b)
{
	return Vector2<T>(a) += b;
}

template<typename T>
ZE_FORCEINLINE Vector2<T> operator+(const Vector2<T>& a, const T& b)
{
	return Vector2<T>(a) += b;
}

template<typename T>
ZE_FORCEINLINE Vector2<T> operator*(const Vector2<T>& a, const Vector2<T>& b)
{
	return Vector2<T>(a) *= b;
}

template<typename T>
ZE_FORCEINLINE Vector2<T> operator*(const Vector2<T>& a, const T& b)
{
	return Vector2<T>(a) *= b;
}

template<typename T>
ZE_FORCEINLINE Vector2<T> operator-(const Vector2<T>& a, const Vector2<T>& b)
{
	return Vector2<T>(a) -= b;
}

template<typename T>
ZE_FORCEINLINE Vector2<T> operator-(const Vector2<T>& other)
{
	return Vector2<T>(0) - other;
}

/**
 * Basic template class for a 3D vector
 */
template<typename T>
	requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
struct Vector3
{
	/** Forward vector (+X) */
	static constexpr Vector3 get_forward() { return Vector3(1, 0, 0); }
	
	/** Right vector (+Y) */
	static constexpr Vector3 get_right() { return Vector3(0, 1, 0); }
	
	/** Up vector (+Z) */
	static constexpr Vector3 get_up() { return Vector3(0, 0, 1); }

	union
	{
		struct
		{
			T x;
			T y;
			T z;
		};

		std::array<T, 3> coords;
	};

	constexpr Vector3() : x(0), y(0), z(0) {}
	constexpr Vector3(const T& in_val) : x(in_val), y(in_val), z(in_val) {}
	constexpr Vector3(const T& in_x, const T& in_y, const T& in_z) : x(in_x), y(in_y), z(in_z) {}
	constexpr Vector3(const Vector2<T>& in_vec2, const T& in_z) : x(in_vec2.x), y(in_vec2.y), z(in_z) {}
	
	template<typename ArchiveType>
    void serialize(ArchiveType& in_archive)
    {
		in_archive <=> x;
		in_archive <=> y;
		in_archive <=> z;
	}

	ZE_FORCEINLINE T& operator[](const size_t& in_col)
	{
		return coords[in_col];
	}

	ZE_FORCEINLINE const T& operator[](const size_t& in_col) const
	{
		return coords[in_col];
	}

	ZE_FORCEINLINE Vector3& operator+=(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	ZE_FORCEINLINE Vector3& operator+=(const T& other)
	{
		x += other;
		y += other;
		z += other;
		return *this;
	}

	ZE_FORCEINLINE Vector3& operator*=(const Vector3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	ZE_FORCEINLINE Vector3& operator*=(const T& other)
	{
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	ZE_FORCEINLINE Vector3& operator-=(const Vector3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	ZE_FORCEINLINE bool operator==(const Vector3& other) const
	{
		return x == other.x 
			&& y == other.y 
			&& z == other.z;
	}

	ZE_FORCEINLINE bool operator!=(const Vector3& other) const
	{
		return x != other.x ||
			y != other.y ||
			z != other.z;
	}
};

/**
 * A 3d vector (double-precision)
 */
using Vector3d = Vector3<double>;

/**
 * A 3d vector (single-precision)
 */
using Vector3f = Vector3<float>;

/** Custom operators */
template<typename T>
ZE_FORCEINLINE Vector3<T> operator+(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>(a) += b;
}

template<typename T>
ZE_FORCEINLINE Vector3<T> operator+(const Vector3<T>& a, const T& b)
{
	return Vector3<T>(a) += b;
}

template<typename T>
ZE_FORCEINLINE Vector3<T> operator*(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>(a) *= b;
}

template<typename T>
ZE_FORCEINLINE Vector3<T> operator*(const Vector3<T>& a, const T& b)
{
	return Vector3<T>(a) *= b;
}

template<typename T>
ZE_FORCEINLINE Vector3<T> operator-(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>(a) -= b;
}

template<typename T>
ZE_FORCEINLINE Vector3<T> operator-(const Vector3<T>& other)
{
	return Vector3<T>(0) - other;
}

/** Custom operators */
ZE_FORCEINLINE Vector3d& operator+=(Vector3d& a, const Vector3f& b)
{
	a += Vector3d(b.x, b.y, b.z);
	return a;
}

ZE_FORCEINLINE Vector3d& operator-=(Vector3d& a, const Vector3f& b)
{
	a -= Vector3d(b.x, b.y, b.z);
	return a;
}

ZE_FORCEINLINE Vector3d operator+(const Vector3d& a, const Vector3f& b)
{
	return Vector3d(a) += Vector3d(b.x, b.y, b.z);
}

ZE_FORCEINLINE Vector3d operator-(const Vector3d& a, const Vector3f& b)
{
	return Vector3d(a) -= Vector3d(b.x, b.y, b.z);
}

/** Functions for operating on vectors */

template<typename T>
ZE_FORCEINLINE T length(const Vector3<T>& in_vec)
{
	return std::sqrt(in_vec.x * in_vec.x + in_vec.y * in_vec.y + in_vec.z * in_vec.z);
}

template<typename T>
ZE_FORCEINLINE Vector3<T> normalize(const Vector3<T>& in_vec)
{
	T length = maths::length(in_vec);
	return Vector3<T>(in_vec.x / length, in_vec.y / length, in_vec.z / length);
}

template<typename T>
ZE_FORCEINLINE Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3(
		a.y * b.z - b.y * a.z,
		a.z * b.x - b.z * a.x,
		a.x * b.y - b.x * a.y);
}

template<typename T>
ZE_FORCEINLINE T dot(const Vector3<T>& a, const Vector3<T>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/** Vector4 */
template<typename T>
	requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
struct Vector4
{
	union 
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};
		
		std::array<T, 4> coords;
	};

	constexpr Vector4() : x(0), y(0), z(0), w(0) {}
	constexpr Vector4(const T& in_val) : x(in_val), y(in_val), z(in_val), w(in_val) {}
	constexpr Vector4(const T& in_x, const T& in_y, const T& in_z, const T& in_w) : x(in_x), y(in_y), z(in_z), w(in_w) {}
	constexpr Vector4(const Vector3<T>& in_vec3, const T& in_w) : x(in_vec3.x), y(in_vec3.y), z(in_vec3.z), w(in_w) {}

	ZE_FORCEINLINE Vector4& operator+(const Vector4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	ZE_FORCEINLINE Vector4& operator*=(const Vector4<T>& in_other)
	{
		x *= in_other.x;
		y *= in_other.y;
		z *= in_other.z;
		w *= in_other.w;
		return *this;
	}
	
	ZE_FORCEINLINE Vector3<T>& operator*=(const Vector3<T>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	ZE_FORCEINLINE Vector4& operator*=(const T& other)
	{
		x *= other;
		y *= other;
		z *= other;
		w *= other;
		return *this;
	}

	ZE_FORCEINLINE T& operator[](const size_t& in_col)
	{
		return coords[in_col];
	}

	ZE_FORCEINLINE const T& operator[](const size_t& in_col) const
	{
		return coords[in_col];
	}
};

template<typename T>
ZE_FORCEINLINE Vector4<T> operator*(const Vector4<T>& a, const Vector4<T>& b)
{
	return Vector4<T>(a) *= b;
}

template<typename T>
ZE_FORCEINLINE Vector4<T> operator*(const Vector4<T>& a, const Vector3<T>& b)
{
	return Vector4<T>(a) *= b;
}

template<typename T>
ZE_FORCEINLINE Vector4<T> operator*(const Vector4<T>& a, const T& b)
{
	return Vector4<T>(a) *= b;
}

} /* namespace ZE::Math */

namespace std
{
	template<typename T>
	ze::maths::Vector2<T> max(const ze::maths::Vector2<T>& a, const ze::maths::Vector2<T>& b)
	{
		return a.x + a.y > b.x + b.y ? a : b; 
	}

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