#pragma once

#include "EngineCore.h"
#include "FileSystem.h"
#include <filesystem>

namespace ze::filesystem
{

/**
 * File system using std library
 */
class ZEFS_API StdFileSystem final : public FileSystem
{
public:
	StdFileSystem(const std::string& in_alias,
		const uint8_t& in_priority, const std::string& in_root);

	OwnerPtr<std::streambuf> read(const std::filesystem::path& path, const FileReadFlags& flags) override;
	OwnerPtr<std::streambuf> write(const std::filesystem::path& path, const FileWriteFlags& flags) override;

	bool iterate_directories(const std::filesystem::path& path,
		const DirectoryIterator& iterator, const IterateDirectoriesFlags& flags) override;

	bool exists(const std::filesystem::path& path) override;
	bool is_directory(const std::filesystem::path& path) override;

	bool is_read_only() const override { return false; }
private:
	std::filesystem::path get_correct_path(const std::filesystem::path& path) const;
private:
	std::filesystem::path root;
};

}