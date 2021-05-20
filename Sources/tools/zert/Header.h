#pragma once

#include "Class.h"
#include "Enum.h"
#include <vector>
#include <filesystem>

/**
 * Represents a C++ header
 */
struct Header
{
	std::string module_name;
	std::filesystem::path path;
	std::vector<Class> classes;
	std::vector<Enum> enums;

	Header(const std::string& in_module_name, const std::filesystem::path& in_path) : module_name(in_module_name), path(in_path) {}

	bool has_refl_data() const
	{
		return !classes.empty() || !enums.empty();
	}
};