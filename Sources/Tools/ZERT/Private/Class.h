#pragma once

#include "Type.h"
#include <unordered_map>

struct Property
{
	std::string type;
	std::string name;
	std::unordered_map<std::string, std::string> metadatas;

	Property(const std::string& in_type, const std::string& in_name) :
		type(in_type), name(in_name) {}
};

class Class : public Type
{
public:
	Class(const bool in_struct, const size_t& in_body_line, std::string in_name, std::string in_namespace = "") 
		: zstruct(in_struct), body_line(in_body_line), Type(ReflType::Class, in_name, in_namespace) {}

	void add_parent(const std::string& in_parent) { parents.emplace_back(in_parent); }
	void add_ctor(const std::string& in_ctor) { ctors.emplace_back(in_ctor); }

	template<typename... Args>
	Property& add_property(Args&&... in_args) { return properties.emplace_back(std::forward<Args>(in_args)...); }

	const size_t get_body_line() const { return body_line; }
	const auto& get_parents() const { return parents; }
	const auto& get_ctors() const { return ctors; }
	const auto& get_properties() const { return properties; }
	const auto& is_struct() const { return zstruct; }
private:
	bool zstruct;
	size_t body_line;
	std::vector<std::string> parents;
	std::vector<std::string> ctors;
	std::vector<Property> properties;
};