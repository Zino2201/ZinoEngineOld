#include "FileSystem/StdFileSystem.h"

namespace ZE::FileSystem
{

DECLARE_LOG_CATEGORY(StdFileSystem);

/** File */
CStdFile::CStdFile(const std::string_view& InPath,
	const bool& bInIsReading) : bIsReading(bInIsReading), Size(0)
{
	if(bInIsReading)
		Stream.open(InPath.data(), std::ios::in | std::ios::ate);
	else
		Stream.open(InPath.data(), std::ios::out | std::ios::ate);

	Size = Stream.tellg();
	Stream.seekg(0);
}

CStdFile::~CStdFile()
{
	if(Stream.is_open())
		Stream.close();
}

void CStdFile::Seek(const int64_t& InPos)
{
	Stream.seekg(InPos);
}

void CStdFile::Write(const uint8_t* InData, const uint64_t& InSize)
{
	Stream.write(reinterpret_cast<const char*>(InData), InSize);
}

void CStdFile::Read(uint8_t* InData, const uint64_t& InSize)
{
	Stream.read(reinterpret_cast<char*>(InData), InSize);
}

/** FileSystem */

CStdFileSystem::CStdFileSystem(const std::string& InAlias,
	const uint8_t& InPriority, const std::string& InPath) : IFileSystem(InAlias,
		InPriority), Root(InPath)
{
	LOG(ELogSeverity::Debug, StdFileSystem, "New std file system: %s", InPath.c_str());
}

TOwnerPtr<IFile> CStdFileSystem::Read(const std::string_view& InPath)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	TOwnerPtr<CStdFile> File = new CStdFile(Path.string(), true);
	if (!File->GetStream().is_open())
	{
		LOG(ELogSeverity::Error, StdFileSystem, "Failed to open file %s",
			InPath.data());
		delete File;
		File = nullptr;
	}

	return File;
}

TOwnerPtr<IFile> CStdFileSystem::Write(const std::string_view& InPath)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	TOwnerPtr<CStdFile> File = new CStdFile(Path.string(), false);
	if(!File->GetStream().is_open())
	{
		LOG(ELogSeverity::Error, StdFileSystem, "Failed to open file %s", 
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

	if(!InIt)
		return false;

	for (auto& Entry : std::filesystem::directory_iterator(Path))
		InIt.Execute(SDirectoryEntry(Entry.path().string()));

	return true;
}

bool CStdFileSystem::Exists(const std::string_view& InPath)
{
	return std::filesystem::exists(GetCorrectPath(InPath));
}

std::filesystem::path CStdFileSystem::GetCorrectPath(const std::string_view& InPath) const
{
	std::filesystem::path Path = InPath;
	if(Path.is_relative())
		Path = Root + std::string(InPath);

	return Path;
}

}