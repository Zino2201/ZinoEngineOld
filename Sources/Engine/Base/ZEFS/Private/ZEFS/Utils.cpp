#include "ZEFS/Utils.h"
#include "ZEFS/ZEFS.h"
#include "ZEFS/FileStream.h"
#include <filesystem>

namespace ze::filesystem
{

std::string read_file_to_string(const std::string_view& path)
{
	std::vector<uint8_t> Array = read_file_to_vector(path);

	return std::string(Array.begin(), Array.end());
}

std::vector<uint8_t> read_file_to_vector(const std::string_view& path, const bool binary)
{
	/** Start at the end to get the file size */
	FileReadFlags flags = FileReadFlagBits::End;
	if(binary)
		flags |= FileReadFlagBits::Binary;

	FileIStream fs(path, flags);
	if (!fs)
		return {};

	int64_t size = fs.tellg();

	/** Go back to beginning */
	fs.seekg(0, std::ios::beg);

	std::vector<uint8_t> array;
	array.resize(size / sizeof(uint8_t));

	fs.read(reinterpret_cast<char*>(array.data()), size);

	return array;
}

}