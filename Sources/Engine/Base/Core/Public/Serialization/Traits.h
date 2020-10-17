#pragma once

namespace ze::serialization
{

/** Type traits for archives */
namespace traits 
{
struct TextArchive {};
}

template<typename T>
static constexpr bool IsInputArchive = std::is_base_of_v<InputArchive<T>, T>;

template<typename T>
static constexpr bool IsOutputArchive = std::is_base_of_v<OutputArchive<T>, T>;

template<typename T>
static constexpr bool IsArchive = IsInputArchive<T> || IsOutputArchive<T>;

template<typename T>
static constexpr bool IsTextArchive = std::is_base_of_v<traits::TextArchive, T>;

}