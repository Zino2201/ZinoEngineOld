#pragma once

#include "EngineCore.h"
#include "Delegates/Delegate.h"
#include <filesystem>

namespace ze::filesystem
{

/**
 * File open flags
 */
enum class FileReadFlagBits
{
	None = 0,

	/** Open in binary mode */
	Binary = 1 << 0,

	/** Place the cursor at the end */
	End = 1 << 1,
};
ENABLE_FLAG_ENUMS(FileReadFlagBits, FileReadFlags);

/**
 * File write flags
 */
enum class FileWriteFlagBits
{
	None = 0,

	ReplaceExisting = 1 << 0,
	Binary = 1 << 1,
};
ENABLE_FLAG_ENUMS(FileWriteFlagBits, FileWriteFlags);

/*
 * Attributes of a file
 */
enum class FileAttributeFlagBits
{
	Hidden = 1 << 0
};
ENABLE_FLAG_ENUMS(FileAttributeFlagBits, FileAttributeFlags)

/** Iteration related types */

enum class IterateDirectoriesFlagBits
{
	None = 0,

	Recursive = 1 << 0,
};
ENABLE_FLAG_ENUMS(IterateDirectoriesFlagBits, IterateDirectoriesFlags);

struct DirectoryEntry
{
	std::filesystem::path path;

	DirectoryEntry(const std::filesystem::path& in_path) : path(in_path) {}
};
using DirectoryIterator = Delegate<void, const DirectoryEntry&>;

/**
 * Interface for a FileSystem
 */
class ZEFS_API FileSystem
{
public:
	FileSystem(const std::string& in_alias,
		const uint8_t& in_priority) {}
	virtual ~FileSystem() = default;

	virtual OwnerPtr<std::streambuf> read(const std::filesystem::path& path, const FileReadFlags& flags) = 0;
	virtual OwnerPtr<std::streambuf> write(const std::filesystem::path& path, const FileWriteFlags& flags) = 0;
	virtual bool iterate_directories(const std::filesystem::path& path,
		const DirectoryIterator& iterator, const IterateDirectoriesFlags& flags) = 0;
	virtual bool exists(const std::filesystem::path& path) = 0;
	virtual bool is_directory(const std::filesystem::path& path) = 0;
	virtual bool is_read_only() const = 0;
	virtual FileAttributeFlags get_file_attributes(const std::filesystem::path& path) const = 0;
	virtual bool set_file_attributes(const std::filesystem::path& path, const FileAttributeFlags& in_flags) = 0;
};

}