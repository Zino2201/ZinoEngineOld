#pragma once

#include "EngineCore.h"
#include "Delegates/Delegate.h"
#include <filesystem>

namespace ZE::FileSystem
{

/**
 * File open flags
 */
enum class EFileReadFlagBits
{
	None = 0,

	/** Open in binary mode */
	Binary = 1 << 0,

	/** Place the cursor at the end */
	End = 1 << 1,
};
ENABLE_FLAG_ENUMS(EFileReadFlagBits, EFileReadFlags);

/**
 * File write flags
 */
enum class EFileWriteFlagBits
{
	None = 0,

	ReplaceExisting = 1 << 0,
	Binary = 1 << 1,
};
ENABLE_FLAG_ENUMS(EFileWriteFlagBits, EFileWriteFlags);

/** Iteration related types */

enum class EIterateDirectoriesFlagBits
{
	None = 0,

	Recursive = 1 << 0,
};
ENABLE_FLAG_ENUMS(EIterateDirectoriesFlagBits, EIterateDirectoriesFlags);

struct SDirectoryEntry
{
	std::filesystem::path Path;

	SDirectoryEntry(const std::filesystem::path& InPath) : Path(InPath) {}
};
using TDirectoryIterator = TDelegate<void, const SDirectoryEntry&>;

/**
 * Interface for a FileSystem
 */
class ZEFS_API IFileSystem
{
public:
	IFileSystem(const std::string& InAlias,
		const uint8_t& InPriority) {}
	virtual ~IFileSystem() = default;

	virtual TOwnerPtr<std::streambuf> Read(const std::filesystem::path& InPath, const EFileReadFlags& InFlags) = 0;
	virtual TOwnerPtr<std::streambuf> Write(const std::filesystem::path& InPath, const EFileWriteFlags& InFlags) = 0;
	virtual bool IterateDirectories(const std::filesystem::path& InPath,
		const TDirectoryIterator& InIt, const EIterateDirectoriesFlags& InFlags) = 0;
	virtual bool Exists(const std::filesystem::path& InPath) = 0;
	virtual bool IsDirectory(const std::filesystem::path& InPath) = 0;
	virtual bool IsReadOnly() const = 0;
};

}