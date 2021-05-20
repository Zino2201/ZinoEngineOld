#include "ZERT.h"
#include "Type.h"

std::vector<Type> types;

void typedb_register(Type&& in_type)
{
	types.emplace_back(std::move(in_type));
}

std::vector<Type> typedb_get_types()
{
	return types;
}

bool typedb_has_type(const std::string& in_type)
{
	for(const auto& type : types)
		if(type.name == in_type)
			return true;

	return false;
}