#include "ZEFS/Paths.h"
#include <filesystem>

namespace ZE::FileSystem::Paths
{

std::string GetCurrentWorkingDir()
{
	return std::filesystem::current_path().string();
}

}