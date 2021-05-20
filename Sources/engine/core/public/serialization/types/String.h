#pragma once

#include "serialization/Archive.h"
#include <string>

namespace ze::serialization
{

template<typename Archive, size_t N>
ZE_FORCEINLINE void serialize(Archive& archive, char (&char_arr)[N])
{
	archive <=> make_binary_data(char_arr, sizeof(char) * N);
}

template<typename Archive>
ZE_FORCEINLINE void serialize(Archive& archive, std::string& string)
{
	std::string::size_type size = string.size();
	
	archive <=> make_size(size);
	
	if constexpr(is_input_archive<Archive>)
		string.resize(size);

	archive <=> make_binary_data(string.data(), size);
}

}