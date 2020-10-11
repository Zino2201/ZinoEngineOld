#pragma once

namespace ze::reflection
{

template<typename T>
static constexpr bool IsReflType = false;

template<typename T>
static constexpr bool IsReflClass = false;

template<typename T>
static constexpr bool IsReflEnum = false;

template<typename T>
	requires IsReflType<T>
static constexpr const char* type_name = "";

}