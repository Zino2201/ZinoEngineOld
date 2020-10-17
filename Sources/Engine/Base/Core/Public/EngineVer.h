#pragma once

#include "Serialization/Archive.h"
#include <fmt/format.h>

namespace ze
{

struct ZEVersion
{
	uint8_t major;
	uint8_t minor;
	uint8_t patch;

	constexpr ZEVersion() : major(0), minor(0), patch(0) {}
	constexpr ZEVersion(const uint8_t& in_major,
		const uint8_t& in_minor,
		const uint8_t& in_patch) : major(in_major),
		minor(in_minor), patch(in_patch) {}

	bool is_compatible_with(const ZEVersion& other) const
	{
		return minor >= other.minor &&
			patch >= other.patch;
	}

	template<typename Archive>
	ZE_FORCEINLINE void serialize(Archive& archive)
	{
		archive <=> major;
		archive <=> minor;
		archive <=> patch;
	}
};

constexpr ZEVersion get_version() 
{
	return ZEVersion(0, 1, 0);
}

}