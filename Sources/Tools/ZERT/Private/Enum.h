#pragma once

class Enum : public Type
{
public:
	Enum(std::string in_name, std::string in_namespace = "") : Type(ReflType::Enum, in_name, in_namespace) {}

	void add_value(const std::string& in_value) { values.emplace_back(in_value); }

	std::vector<std::string> values;
};