#pragma once

#include "MathCore.h"
#include "Vector.h"

namespace ze::maths
{

/**
 * A fixed size matrix 4x4
 * Column-major
 */
template<typename T>
struct Matrix4
{
	using ColumnType = maths::Vector4<T>;

	Matrix4() : columns(
		{
			Vector4<T>(1, 0, 0, 0),
			Vector4<T>(0, 1, 0, 0),
			Vector4<T>(0, 0, 1, 0),
			Vector4<T>(0, 0, 0, 1),
		}) {}

	Matrix4(const T& in_val) : columns(
		{
			Vector4<T>(in_val, 0, 0, 0),
			Vector4<T>(0, in_val, 0, 0),
			Vector4<T>(0, 0, in_val, 0),
			Vector4<T>(0, 0, 0, in_val),
		}) {}

	Matrix4(const Matrix4& in_other) : columns(
		{
			in_other[0],
			in_other[1],
			in_other[2],
			in_other[3],
		}) {}

	void operator=(const Matrix4& in_other)
	{
		memcpy(columns.data(), in_other.columns.data(), sizeof(columns));
	}

	ZE_FORCEINLINE auto& operator[](const size_t& in_col)
	{
		return columns[in_col];
	}

	ZE_FORCEINLINE const auto& operator[](const size_t& in_col) const
	{
		return columns[in_col];
	}
private:
	std::array<ColumnType, 4> columns;
};

template<typename T>
ZE_FORCEINLINE Matrix4<T> operator*(const Matrix4<T>& in_a, const Matrix4<T>& in_b)
{
	const Vector4<T> src_a0 = in_a[0];
	const Vector4<T> src_a1 = in_a[1];
	const Vector4<T> src_a2 = in_a[2];
	const Vector4<T> src_a3 = in_a[3];

	const Vector4<T> src_b0 = in_b[0];
	const Vector4<T> src_b1 = in_b[1];
	const Vector4<T> src_b2 = in_b[2];
	const Vector4<T> src_b3 = in_b[3];

	Matrix4<T> ret;
	ret[0] = src_a0 * src_b0[0] + src_a1 * src_b0[1] + src_a2 * src_b0[2] + src_a3 * src_b0[3];
	ret[1] = src_a0 * src_b1[0] + src_a1 * src_b1[1] + src_a2 * src_b1[2] + src_a3 * src_b1[3];
	ret[2] = src_a0 * src_b2[0] + src_a1 * src_b2[1] + src_a2 * src_b2[2] + src_a3 * src_b2[3];
	ret[3] = src_a0 * src_b3[0] + src_a1 * src_b3[1] + src_a2 * src_b3[2] + src_a3 * src_b3[3];
	return ret;
}

using Matrix4f = Matrix4<float>;
using Matrix4d = Matrix4<double>;

}