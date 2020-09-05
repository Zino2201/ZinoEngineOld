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

	TOwnerPtr<std::streambuf> Read(const std::string_view& InPath, const EFileReadFlags& InFlags) override;
	TOwnerPtr<std::streambuf> Write(const std::string_view& InPath, const EFileWriteFlags& InFlags) override;

	bool IterateDirectories(const std::string_view& InPath,
		const TDirectoryIterator& InIt) override;

	bool Exists(const std::string_view& InPath) override;
	bool IsDirectory(const std::string_view& InPath) override;

	bool IsReadOnly() const override { return false; }
private:
	std::filesystem::path GetCorrectPath(const std::string_view& InPath) const;
private:
	std::filesystem::path Root;
};

}