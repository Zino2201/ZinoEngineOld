#pragma once

namespace ze::serialization
{

template<typename Archive, typename T1, typename T2>
void serialize(Archive& archive, std::pair<T1, T2>& pair)
{
	archive <=> pair.first;
	archive <=> pair.second;
}

}