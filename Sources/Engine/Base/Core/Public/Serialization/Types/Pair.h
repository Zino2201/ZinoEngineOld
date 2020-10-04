#pragma once

namespace ZE::Serialization
{

template<typename Archive, typename T1, typename T2>
void Serialize(Archive& InArchive, std::pair<T1, T2>& InValue)
{
	InArchive <=> InValue.first;
	InArchive <=> InValue.second;
}

}