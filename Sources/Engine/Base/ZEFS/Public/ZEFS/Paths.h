#pragma once

#include "EngineCore.h"

/**
 * Functions to manipulate paths
 */
namespace ze::filesystem
{

/**
 * Get current working directory
 */
ZEFS_API std::string get_current_working_dir();

}