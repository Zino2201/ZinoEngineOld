#pragma once

#include "Serialization/Archive.h"
#include <vector>

namespace ze::serialization
{

template<typename Archive, typename T>
ZE_FORCEINLINE void serialize(Archive& archive, std::vector<T>& vector)
{
	typename std::vector<T>::size_type size = vector.size();

	archive <=> make_size(size);
	if constexpr (is_input_archive<Archive>)
		vector.resize(size);

	for (auto& elem : vector)
		archive <=> elem;
}

}