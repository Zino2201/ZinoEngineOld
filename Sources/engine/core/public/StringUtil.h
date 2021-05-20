#pragma once

#include "MinimalMacros.h"
#include <string>
#include <sstream>
#include <vector>

namespace ze::stringutil
{


/**
 * Split a string using the following delimiter
 */
CORE_API std::vector<std::string> split(const std::string& string, 
	const char& delimiter);

}