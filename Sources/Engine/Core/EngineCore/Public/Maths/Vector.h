#pragma once

#include "MathCore.h"

namespace ZE::Math
{

/**
 * Base template class for a 2D vector
 */
template<typename T>
struct TVector2
{
	T X;
	T Y;

	TVector2() : X(0), Y(0) {}
	TVector2(const T& InX) : X(InX), Y(InX) {}
	TVector2(const T& InX, const T& InY) : X(InX), Y(InY) {}
};

using SVector2f = TVector2<float>;

/**
 * Basic template class for a 3D vector
 */
template<typename T>
struct TVector3
{
	T X;
	T Y;
	T Z;

	TVector3() : X(0), Y(0), Z(0) {}
	TVector3(const T& InX, const T& InY, const T& InZ) : X(InX), Y(InY), Z(InZ) {}

	bool operator==(const TVector3<T>& InOther) const
	{
		return X == InOther.X &&
			Y == InOther.Y &&
			Z == InOther.Z;
	}
};

/**
 * A 3d vector (double-precision)
 */
struct SVector3 : public TVector3<double>
{
	SVector3& operator=(const glm::vec3& InVec)
	{
		X = InVec.x;
		Y = InVec.x;
		Z = InVec.z;

		return *this;
	}
};

/**
 * A 3d vector (single-precision)
 */
struct SVector3Float : public TVector3<float>
{
	SVector3Float() : TVector3<float>() {}
	SVector3Float(float InX, float InY, float InZ) : TVector3<float>(InX, InY, InZ) {}
};

struct SVector3FloatHash
{
	std::size_t operator()(const SVector3Float& InVec) const noexcept
	{
		std::size_t Seed = 0;

		HashCombine(Seed, InVec.X);
		HashCombine(Seed, InVec.Y);
		HashCombine(Seed, InVec.Z);

		return Seed;
	}
};

} /* namespace ZE::Math */