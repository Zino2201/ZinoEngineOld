#pragma once

namespace ze::reflection
{

template<typename T>
static constexpr bool is_refl_type = false;

template<typename T>
static constexpr bool is_refl_class = false;

template<typename T>
static constexpr bool is_refl_enum = false;

template<typename T>
	requires is_refl_type<T>
static constexpr const char* type_name = "";

}