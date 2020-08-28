#pragma once

#include "EngineCore.h"
#include <string>

/**
 * Utilities for files and paths
 */
namespace ZE
{

namespace FileUtils
{
CORE_API std::string GetCurrentWorkingDirectory();
CORE_API std::string GetShadersDirectory();
CORE_API void ReadTextFile(const std::string_view& InPath,
	std::string& InString);
} 

namespace PathUtils
{

CORE_API std::string GetFilename(const std::string_view& InPath);

}

}