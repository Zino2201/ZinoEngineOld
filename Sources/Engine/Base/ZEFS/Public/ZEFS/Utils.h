#pragma once

#include "EngineCore.h"

/**
 * Utils functions for file manipulation
 */

namespace ZE::FileSystem
{

ZEFS_API std::string ReadFileToString(const std::string_view& InPath);
ZEFS_API std::vector<uint8_t> ReadFileToVector(const std::string_view& InPath);

}