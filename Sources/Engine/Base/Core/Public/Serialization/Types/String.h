#pragma once

#include "Serialization/Archive.h"
#include <string>

namespace ZE::Serialization
{

template<typename Archive, size_t N>
ZE_FORCEINLINE void Serialize(Archive& InArchive, char (&InChar)[N])
{
	InArchive <=> MakeBinaryData(InChar, sizeof(char) * N);
}

template<typename Archive>
ZE_FORCEINLINE void Serialize(Archive& InArchive, std::string& InString)
{
	std::string::size_type Size = InString.size();
	
	InArchive <=> MakeSize(Size);
	if constexpr(TIsInputArchive<Archive>)
		InString.resize(Size);

	InArchive <=> MakeBinaryData(InString.data(), Size);
}

}