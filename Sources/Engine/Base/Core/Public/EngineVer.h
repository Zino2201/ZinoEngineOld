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

	template<typename Archive>
	ZE_FORCEINLINE void Serialize(Archive& InArchive)
	{
		InArchive <=> Major;
		InArchive <=> Minor;
		InArchive <=> Patch;
	}
};

constexpr SZEVersion GetZEVersion() 
{
	return SZEVersion(0, 1, 0);
}

}