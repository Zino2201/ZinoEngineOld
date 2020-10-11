#pragma once

#include "EngineCore.h"
#include "Flags/Flags.h"
#include "Type.h"
#include "Any.h"
#include <string>
#include <cstdint>

namespace ze::reflection
{

enum class PropertyFlagBits
{
	None = 0,

	Serializable = 1 << 0,
};
ENABLE_FLAG_ENUMS(PropertyFlagBits, PropertyFlags)

class Property
{
public:
	Property(const std::string& in_name,
		const std::string& in_type_name,
		const size_t& in_offset,
		const PropertyFlags& in_flags) : name(in_name), type(in_type_name),
		offset(in_offset), flags(in_flags) {}

	Any get_value(const void* instance) const;
private:
	std::string name;
	LazyTypePtr type;
	size_t offset;
	PropertyFlags flags;
};

}