#include "ZEFS/Paths.h"
#include <filesystem>

namespace ZE::FileSystem::Paths
{

std::string GetCurrentWorkingDir()
{
	return std::filesystem::current_path().string();
}

std::string GetFilename(const std::string& InPath)
{
	return std::filesystem::path(InPath).filename().string();
}

}