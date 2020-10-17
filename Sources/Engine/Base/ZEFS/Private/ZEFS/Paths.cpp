#include "ZEFS/Paths.h"
#include <filesystem>

namespace ze::filesystem
{

std::string get_current_working_dir()
{
	return std::filesystem::current_path().string();
}

}