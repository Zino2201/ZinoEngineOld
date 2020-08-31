#pragma once

#include "Serialization/Archive.h"
#include <vector>

namespace ZE::Serialization
{

template<typename Archive, typename T>
ZE_FORCEINLINE void Serialize(Archive& InArchive, std::vector<T>& InValue)
{
	std::vector<T>::size_type Size = InValue.size();

	InArchive <=> MakeSize(Size);
	if constexpr (TIsInputArchive<Archive>)
		InValue.resize(Size);

	for (auto& Elem : InValue)
		InArchive <=> Elem;
}

}