#pragma once

#include <uuid.h>
#include "String.h"

namespace ze::serialization
{

template<typename Archive>
ZE_FORCEINLINE void serialize(Archive& archive, uuids::uuid& uuid)
{
	if constexpr(is_input_archive<Archive>)
	{
		std::string uuid_str;
		archive <=> uuid_str;
		uuid.from_string(uuid_str);
	}
	else
	{
		archive <=> uuids::to_string(uuid);
	}
}

}