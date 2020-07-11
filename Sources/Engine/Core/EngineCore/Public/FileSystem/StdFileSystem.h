#pragma once

#include "FileSystem.h"
#include <filesystem>
#include <fstream>

namespace ZE::FileSystem
{

/**
 * Std file
 */
class ENGINECORE_API CStdFile final : public IFile
{
public:
	CStdFile(const std::string_view& InPath,
		const bool& bInIsReading,
		const bool& bInBinary);
	~CStdFile();

	void Seek(const int64_t& InPos) override;
	void Read(uint8_t* InData, const uint64_t& InSize) override;
	void Write(const uint8_t* InData, const uint64_t& InSize) override;
	void Flush() override;
	const std::fstream& GetStream() const { return Stream; }
	bool IsReading() const override { return bIsReading; }
	int64_t GetSize() const override { return Size; }
	int64_t Tell() override { return Stream.tellg(); }
	bool IsBinary() const override { return bIsBinary; }
private:
	std::fstream Stream;
	bool bIsReading;
	bool bIsBinary;
	int64_t Size;
};

/**
 * Standard file system using std lib
 */
class ENGINECORE_API CStdFileSystem final : public IFileSystem
{
public:
	CStdFileSystem(const std::string& InAlias,
		const uint8_t& InPriority, const std::string& InPath);

	TOwnerPtr<IFile> Read(const std::string_view& InPath, const bool& bIsBinary) override;
	TOwnerPtr<IFile> Write(const std::string_view& InPath, const bool& bIsBinary) override;

	bool IterateDirectories(const std::string_view& InPath,
		const TDirectoryIterator& InIt) override;

	bool Exists(const std::string_view& InPath) override;

	bool IsReadOnly() const override { return true; }
private:
	std::filesystem::path GetCorrectPath(const std::string_view& InPath) const;
private:
	std::string Root;
};

}