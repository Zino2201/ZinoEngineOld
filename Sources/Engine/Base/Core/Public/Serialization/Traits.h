#pragma once

namespace ZE::Serialization
{

/** Type traits for archives */
namespace Traits 
{
	struct TextArchive {};
}

template<typename T>
static constexpr bool TIsInputArchive = std::is_base_of_v<TInputArchive<T>, T>;

template<typename T>
static constexpr bool TIsOutputArchive = std::is_base_of_v<TOutputArchive<T>, T>;

template<typename T>
static constexpr bool TIsArchive = TIsInputArchive<T> || TIsOutputArchive<T>;

template<typename T>
static constexpr bool TIsTextArchive = std::is_base_of_v<Traits::TextArchive, T>;

}