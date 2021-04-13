#pragma once

namespace ze::serialization
{

/** Type traits for archives */
namespace traits 
{
struct TextArchive {};
}

template<typename T>
static constexpr bool is_input_archive = std::is_base_of_v<InputArchive<T>, T>;

template<typename T>
static constexpr bool is_output_archive = std::is_base_of_v<OutputArchive<T>, T>;

template<typename T>
static constexpr bool is_archive = is_input_archive<T> || is_output_archive<T>;

template<typename T>
static constexpr bool is_text_archive = std::is_base_of_v<traits::TextArchive, T>;

}