#pragma once

namespace ze::serialization
{

/** Type traits for archives */
namespace traits 
{

struct TextArchive {};

/** A archive that contains another archive, bypass verifications on <=> operators */
struct ContainerArchive {};

}

template<typename T>
static constexpr bool is_text_archive = std::is_base_of_v<traits::TextArchive, T>;

template<typename T>
static constexpr bool is_container_archive = std::is_base_of_v<traits::ContainerArchive, T>;

}