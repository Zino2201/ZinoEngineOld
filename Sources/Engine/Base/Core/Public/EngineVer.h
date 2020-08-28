#pragma once

#include "Serialization/Archive.h"

namespace ZE
{

struct SZEVersion
{
	uint8_t Major;
	uint8_t Minor;
	uint8_t Patch;

	constexpr SZEVersion(const uint8_t& InMajor,
		const uint8_t& InMinor,
		const uint8_t& InPatch) : Major(InMajor),
		Minor(InMinor), Patch(InPatch) {}

	bool IsCompatibleWith(const SZEVersion& InOther) const
	{
		return Minor >= InOther.Minor &&
			Patch >= InOther.Patch;
	}
};

constexpr SZEVersion GetZEVersion() 
{
	return SZEVersion(0, 1, 0);
}

/** Serialization operators */
template<typename Archive>
FORCEINLINE void Serialize(Archive& InArchive, const SZEVersion& InValue)
{
	InArchive <=> InValue.Major;
	InArchive <=> InValue.Minor;
	InArchive <=> InValue.Patch;

	return InArchive;
}

}