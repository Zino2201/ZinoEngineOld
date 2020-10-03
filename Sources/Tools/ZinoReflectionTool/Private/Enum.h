#pragma once

#include "ZRT.h"

class CEnum
{
public:
	CEnum(const std::string& InNamespace,
		const std::string& InName) : Namespace(InNamespace),
		Name(InName) {}

	void AddValue(const std::string& InName) { Values.emplace_back(InName); }

	const std::string& GetNamespace() const { return Namespace; }
	const std::string& GetName() const { return Name; }
	const auto& GetValues() const { return Values; }
private:
	std::string Namespace;
	std::string Name;
	std::vector<std::string> Values;
};