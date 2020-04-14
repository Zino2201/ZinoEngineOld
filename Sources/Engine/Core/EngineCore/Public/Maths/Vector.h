#pragma once

#include "MathCore.h"

namespace ZE::Math
{

/**
 * Basic template class for a 3D vector
 */
template<typename T>
struct TVector3
{
	T X;
	T Y;
	T Z;

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
   
};

/**
 * A 3d vector (single-precision)
 */
struct SVector3Float : public TVector3<float>
{
	
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