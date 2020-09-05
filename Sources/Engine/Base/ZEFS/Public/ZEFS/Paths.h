#pragma once

#include "EngineCore.h"

/**
 * Functions to manipulate paths
 */
namespace ZE::FileSystem::Paths
{
ZEFS_API inline std::string GetCurrentWorkingDir();
ZEFS_API inline std::string GetFilename(const std::string& InPath);

}