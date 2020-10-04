#pragma once

#include "Serialization/Archive.h"
#include "Pair.h"
#include <robin_hood.h>

namespace ZE::Serialization
{

template<typename Archive, typename Key, typename Val>
void Serialize(Archive& InArchive, robin_hood::unordered_map<Key, Val>& InValue)
{
	typename robin_hood::unordered_map<Key, Val>::size_type Size = InValue.size();

	InArchive <=> MakeSize(Size);

	if constexpr(TIsInputArchive<Archive>)
	{
		for(decltype(Size) i = 0; i < Size; ++i)
		{
			std::pair<Key, Val> Pair;
			InArchive <=> Pair;
			InValue.insert({ std::move(Pair.first), std::move(Pair.second) });
		}
	}
	else
	{
		for (auto& Elem : InValue)
			InArchive <=> std::make_pair(Elem.first, Elem.second);
	}
}

}