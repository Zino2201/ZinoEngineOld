#pragma once

#include "EngineCore.h"

/**
 * Utils functions for file manipulation
 */

namespace ze::filesystem
{

ZEFS_API std::string read_file_to_string(const std::string_view& path);
ZEFS_API std::vector<uint8_t> read_file_to_vector(const std::string_view& path);

}