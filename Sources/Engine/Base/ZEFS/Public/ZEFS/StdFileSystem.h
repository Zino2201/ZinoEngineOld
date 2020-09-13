#pragma once

#include "EngineCore.h"
#include "FileSystem.h"
#include <filesystem>

namespace ZE::FileSystem
{

/**
 * File system using std library
 */
class ZEFS_API CStdFileSystem final : public IFileSystem
{
public:
	CStdFileSystem(const std::string& InAlias,
		const uint8_t& InPriority, const std::string& InRoot);

	TOwnerPtr<std::streambuf> Read(const std::filesystem::path& InPath, const EFileReadFlags& InFlags) override;
	TOwnerPtr<std::streambuf> Write(const std::filesystem::path& InPath, const EFileWriteFlags& InFlags) override;

	bool IterateDirectories(const std::filesystem::path& InPath,
		const TDirectoryIterator& InIt, const EIterateDirectoriesFlags& InFlags) override;

	bool Exists(const std::filesystem::path& InPath) override;
	bool IsDirectory(const std::filesystem::path& InPath) override;

	bool IsReadOnly() const override { return false; }
private:
	std::filesystem::path GetCorrectPath(const std::filesystem::path& InPath) const;
private:
	std::filesystem::path Root;
};

}