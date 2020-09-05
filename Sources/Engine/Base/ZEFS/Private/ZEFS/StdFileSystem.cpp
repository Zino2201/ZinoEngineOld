#include "ZEFS/StdFileSystem.h"
#include <fstream>

namespace ZE::FileSystem
{

/** File system */

CStdFileSystem::CStdFileSystem(const std::string& InAlias,
	const uint8_t& InPriority, const std::string& InRoot) : IFileSystem(InAlias,
		InPriority), Root(InRoot) {}

TOwnerPtr<std::streambuf> CStdFileSystem::Read(const std::string_view& InPath, const EFileReadFlags& InFlags)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	TOwnerPtr<std::filebuf> File = new std::filebuf;
	
	uint32_t Flags = std::ios::in;
	if (InFlags & EFileReadFlagBits::Binary)
		Flags |= std::ios::binary;
	
	if (InFlags & EFileReadFlagBits::End)
		Flags |= std::ios::ate;
	
	File->open(InPath, Flags);
	if (!File->is_open())
	{
		ZE::Logger::Error("Failed to open file {}",
			InPath.data());
		delete File;
		File = nullptr;
	}

	return File;
}

TOwnerPtr<std::streambuf> CStdFileSystem::Write(const std::string_view& InPath, const EFileWriteFlags& InFlags)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	TOwnerPtr<std::filebuf> File = new std::filebuf;

	uint32_t Flags = std::ios::out;
	if (InFlags & EFileWriteFlagBits::Binary)
		Flags |= std::ios::binary;

	File->open(InPath, Flags);
	if (!File->is_open())
	{
		ZE::Logger::Error("Failed to open file {}",
			InPath.data());
		delete File;
		File = nullptr;
	}

	return File;
}

bool CStdFileSystem::IterateDirectories(const std::string_view& InPath,
	const TDirectoryIterator& InIt)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	if (!InIt)
		return false;

	for (auto& Entry : std::filesystem::directory_iterator(Path))
		InIt.Execute(SDirectoryEntry(Entry.path().string()));

	return true;
}

bool CStdFileSystem::Exists(const std::string_view& InPath)
{
	return std::filesystem::exists(GetCorrectPath(InPath));
}

bool CStdFileSystem::IsDirectory(const std::string_view& InPath)
{
	return std::filesystem::is_directory(GetCorrectPath(InPath));
}

std::filesystem::path CStdFileSystem::GetCorrectPath(const std::string_view& InPath) const
{
	std::filesystem::path Path = InPath;
	if (Path.is_relative())
		Path = Root / std::string(InPath);

	return Path;
}

}