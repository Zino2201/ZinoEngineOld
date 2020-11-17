#pragma once

#include "Maths/Vector.h"
#include "Maths/Matrix.h"

namespace ze::maths
{

/**
 * useful functions for transformations
 * inspired by https://github.com/g-truc/glm
 */

template<typename T>
	requires std::is_floating_point_v<T>
Matrix4<T> look_at(const Vector3<T>& in_eye, const Vector3<T>& in_target, const Vector3<T>& in_up)
{
	Matrix4<T> mat(1);

	Vector3<T> forward, right, up;
	forward = normalize(in_target - in_eye);
	right = normalize(cross(in_up, forward));
	up = cross(forward, right);

	mat[0][0] = right.x;
	mat[1][0] = right.y;
	mat[2][0] = right.z;

	mat[0][1] = up.x;
	mat[1][1] = up.y;
	mat[2][1] = up.z;

	mat[0][2] = forward.x;
	mat[1][2] = forward.y;
	mat[2][2] = forward.z;
	
	mat[3][0] = -dot(right, in_eye);
	mat[3][1] = -dot(up, in_eye);
	mat[3][2] = -dot(forward, in_eye);

 	return mat;
}

template<typename T>
	requires std::is_floating_point_v<T>
Matrix4<T> perspective(const T in_fov, const T in_aspect_ratio, const T in_near, const T in_far)
{
	Matrix4<T> mat(0);

	const float tan_half_fov = std::tan(in_fov / static_cast<T>(2));

	mat[0][0] = static_cast<T>(1) / (in_aspect_ratio * tan_half_fov);
	mat[1][1] = (static_cast<T>(1) / tan_half_fov) * -1;
	mat[2][2] = in_far / (in_far - in_near);
	mat[2][3] = static_cast<T>(1);
	mat[3][2] = -(in_far * in_near) / (in_far - in_near);

	return mat;
}

template<typename T>
	requires std::is_floating_point_v<T>
Matrix4<T> infinite_perspective(const T in_fov, const T in_aspect_ratio, const T in_near)
{
	Matrix4<T> mat(0);

	const float range = std::tan(in_fov / static_cast<T>(2)) * in_near;
	const float left = -range * in_aspect_ratio;
	const float right = range * in_aspect_ratio;
	const float bottom = -range;
	const float top = range;

	mat[0][0] = (static_cast<T>(2) * in_near) / (right - left);
	mat[1][1] = ((static_cast<T>(2) * in_near) / (top - bottom)) * -1;
	mat[2][2] = static_cast<T>(1);
	mat[2][3] = static_cast<T>(1);
	mat[3][2] = -static_cast<T>(2) * in_near;

	return mat;
}

template<typename T>
	requires std::is_floating_point_v<T>
Matrix4<T> translate(const Vector3<T>& in_vec)
{
	Matrix4<T> mat(1);
	
	mat[3] = mat[0] * in_vec.x + mat[1] * in_vec.y + mat[2] * in_vec.z;
	mat[3].w = 1;

	return mat;
}

template<typename T>
	requires std::is_floating_point_v<T>
Matrix4<T> scale(const Vector3<T>& in_vec)
{
	Matrix4<T> mat(1);
	
	mat[0] *= in_vec[0];
	mat[1] *= in_vec[1];
	mat[2] *= in_vec[2];

	return mat;
}

}