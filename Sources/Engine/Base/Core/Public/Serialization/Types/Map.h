#pragma once

#include "Serialization/Archive.h"
#include "Pair.h"
#include <robin_hood.h>

namespace ze::serialization
{

template<typename Archive, typename Key, typename Val>
void serialize(Archive& archive, robin_hood::unordered_map<Key, Val>& value)
{
	typename robin_hood::unordered_map<Key, Val>::size_type size = value.size();

	archive <=> make_size(size);

	if constexpr(IsInputArchive<Archive>)
	{
		for(decltype(size) i = 0; i < size; ++i)
		{
			std::pair<Key, Val> pair;
			archive <=> pair;
			value.insert({ std::move(pair.first), std::move(pair.second) });
		}
	}
	else
	{
		for (auto& elem : value)
			archive <=> std::make_pair(elem.first, elem.second);
	}
}

}