#pragma once

enum class ReflType
{
	Primitive,
	Class,
	Enum
};

struct Type
{
	Type(ReflType in_refl_type, std::string in_name, std::string in_namespace = "") :
		znamespace(in_namespace), name(in_name), refl_type(in_refl_type) {}

	/** Namespace (in::this::format) */
	std::string znamespace;
	std::string name;
	ReflType refl_type;
	std::string documentation;
};

void typedb_register(Type&& in_type);
std::vector<Type> typedb_get_types();
bool typedb_has_type(const std::string& in_type);