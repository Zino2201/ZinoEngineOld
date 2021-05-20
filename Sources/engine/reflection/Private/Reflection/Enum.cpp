#include "reflection/Enum.h"

namespace ze::reflection
{

const Enum* get_by_name(const std::string& in_name)
{
	const Type* type = Type::get_by_name(in_name);
	if(type && type->is_enum())
		return static_cast<const Enum*>(type);

	return nullptr;
}

std::string Enum::get_value_name(const Any& in_value) const
{
	for(const auto& [name, value] : values)
	{
		if(value == in_value)
			return name;
	}

	return std::string();
}

}