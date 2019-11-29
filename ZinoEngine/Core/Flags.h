#pragma once

#include <flags/flags.hpp>

#define DECLARE_FLAG_ENUM(EnumType) ALLOW_FLAGS_FOR_ENUM(EnumType) \
	using EnumType##Flags = flags::flags<EnumType>;