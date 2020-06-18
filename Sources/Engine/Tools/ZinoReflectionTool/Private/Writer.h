#pragma once

#include "ZRT.h"

class CHeader;

/**
 * Write generated files from a CHeader
 */
namespace Writer
{
	void Write(const std::string_view& InOutDir, const CHeader& InHeader);
};