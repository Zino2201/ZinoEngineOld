#pragma once

#include "MinimalMacros.h"
#include <string>
#include <sstream>
#include <vector>

namespace ZE::StringUtil
{


/**
 * Split a string using the following delimiter
 */
CORE_API std::vector<std::string> Split(const std::string& InString, 
	const char& InDelimiter);

}