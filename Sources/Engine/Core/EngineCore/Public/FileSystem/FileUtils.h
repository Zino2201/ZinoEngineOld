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
ENGINECORE_API std::string GetCurrentWorkingDirectory();
ENGINECORE_API std::string GetShadersDirectory();
ENGINECORE_API void ReadTextFile(const std::string_view& InPath,
	std::string& InString);
} 

namespace PathUtils
{

ENGINECORE_API std::string GetFilename(const std::string_view& InPath);

}

}