#include "FileSystem/StdFileSystem.h"

namespace ZE::FileSystem
{

/** File */
CStdFile::CStdFile(const std::string_view& InPath,
	const bool& bInIsReading,
	const bool& bInBinary) : bIsReading(bInIsReading), Size(0), bIsBinary(bInBinary)
{
	std::ios::openmode OpenMode = std::ios::ate;

	if(bInBinary)
		OpenMode |= std::ios::binary;

	if(bInIsReading)
		OpenMode |= std::ios::in;
	else
		OpenMode |= std::ios::out;

	Stream.open(InPath.data(), OpenMode);

	if(!Stream.is_open())
	{
		char ErrBuf[512];
		UNUSED_VARIABLE(ErrBuf);
		ZE::Logger::Error("{}", strerror_s(ErrBuf, errno));
		return;
	}

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

void CStdFile::Flush()
{
	Stream.flush();
}

/** FileSystem */

CStdFileSystem::CStdFileSystem(const std::string& InAlias,
	const uint8_t& InPriority, const std::string& InPath) : IFileSystem(InAlias,
		InPriority), Root(InPath)
{
	ZE::Logger::Verbose("New std file system: {}", InPath.c_str());
}

TOwnerPtr<IFile> CStdFileSystem::Read(const std::string_view& InPath, const bool& bIsBinary)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	TOwnerPtr<CStdFile> File = new CStdFile(Path.string(), true, bIsBinary);
	if (!File->GetStream().is_open())
	{
		ZE::Logger::Error("Failed to open file {}",
			InPath.data());
		delete File;
		File = nullptr;
	}

	return File;
}

TOwnerPtr<IFile> CStdFileSystem::Write(const std::string_view& InPath, const bool& bIsBinary)
{
	std::filesystem::path Path = GetCorrectPath(InPath);

	TOwnerPtr<CStdFile> File = new CStdFile(Path.string(), false, bIsBinary);
	if(!File->GetStream().is_open())
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