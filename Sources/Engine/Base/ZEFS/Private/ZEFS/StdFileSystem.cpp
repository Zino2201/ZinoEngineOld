#include "ZEFS/StdFileSystem.h"
#include <fstream>

namespace ZE::FileSystem
{

/** File system */

CStdFileSystem::CStdFileSystem(const std::string& InAlias,
	const uint8_t& InPriority, const std::string& InRoot) : IFileSystem(InAlias,
		InPriority), Root(InRoot) {}

TOwnerPtr<std::streambuf> CStdFileSystem::Read(const std::filesystem::path& InPath, const EFileReadFlags& InFlags)
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
			InPath.string());
		delete File;
		File = nullptr;
	}

	return File;
}

TOwnerPtr<std::streambuf> CStdFileSystem::Write(const std::filesystem::path& InPath, const EFileWriteFlags& InFlags)
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
			InPath.string());
		delete File;
		File = nullptr;
	}

	return File;
}

bool CStdFileSystem::IterateDirectories(const std::filesystem::path& InPath,
	const TDirectoryIterator& InIt, const EIterateDirectoriesFlags& InFlags)
{
	std::filesystem::path Path = GetCorrectPath(InPath);
	if (!InIt)
		return false;

	if (InFlags & EIterateDirectoriesFlagBits::Recursive)
	{
		for (auto& Entry : std::filesystem::recursive_directory_iterator(Path))
		{
			InIt.Execute(SDirectoryEntry(std::filesystem::relative(Entry.path(), Path)));
		}
	}
	else
	{
		for (auto& Entry : std::filesystem::directory_iterator(Path))
		{
			InIt.Execute(SDirectoryEntry(std::filesystem::relative(Entry.path(), Path)));
		}
	}

	return true;
}

bool CStdFileSystem::Exists(const std::filesystem::path& InPath)
{
	return std::filesystem::exists(GetCorrectPath(InPath));
}

bool CStdFileSystem::IsDirectory(const std::filesystem::path& InPath)
{
	return std::filesystem::is_directory(GetCorrectPath(InPath));
}

std::filesystem::path CStdFileSystem::GetCorrectPath(const std::filesystem::path& InPath) const
{
	std::filesystem::path Path = InPath;
	if (Path.is_relative())
		Path = Root / InPath;

	return Path;
}

}