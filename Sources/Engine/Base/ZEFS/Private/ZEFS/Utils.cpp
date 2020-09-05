#include "ZEFS/Utils.h"
#include "ZEFS/ZEFS.h"
#include "ZEFS/FileStream.h"
#include <filesystem>

namespace ZE::FileSystem
{

std::string ReadFileToString(const std::string_view& InPath)
{
	/** Start at the end to get the file size */
	CIFileStream FS(InPath, EFileReadFlagBits::End);
	if (!FS)
		return "";

	int64_t Size = FS.tellg();

	/** Go back to beginning */
	FS.seekg(0, std::ios::beg);

	std::vector<uint8_t> Array;
	Array.resize(Size / sizeof(uint8_t));

	FS.read(reinterpret_cast<char*>(Array.data()), Size);

	return std::string(Array.begin(), Array.end());
}

std::vector<uint8_t> ReadFileToVector(const std::string_view& InPath)
{
	/** Start at the end to get the file size */
	CIFileStream FS(InPath, EFileReadFlagBits::End);
	if (!FS)
		return {};

	int64_t Size = FS.tellg();

	/** Go back to beginning */
	FS.seekg(0, std::ios::beg);

	std::vector<uint8_t> Array;
	Array.resize(Size / sizeof(uint8_t));

	FS.read(reinterpret_cast<char*>(Array.data()), Size);

	return Array;
}

}