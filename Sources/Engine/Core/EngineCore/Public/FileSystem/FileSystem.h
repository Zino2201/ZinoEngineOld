#pragma once

#include "EngineCore.h"
#include "Delegates/Delegate.h"

namespace ZE::FileSystem
{

/** Directories */
struct SDirectoryEntry
{
	std::string_view Path;

	SDirectoryEntry(const std::string_view& InPath) : Path(InPath) {}
};
using TDirectoryIterator = TDelegate<void, const SDirectoryEntry&>;

/**
 * Path seperator
 */
static constexpr char GPathSeparator = '\\';

/**
 * Interface to a file
 */
class ENGINECORE_API IFile
{
public:
	virtual ~IFile() = default;

	/**
	 * Seek to a specific position in the file
	 */
	virtual void Seek(const int64_t& InPos) = 0;

	/**
	 * Read from the file
	 */
	virtual void Read(uint8_t* InData, const uint64_t& InSize) = 0;

	/**
	 * Write to the file
	 */
	virtual void Write(const uint8_t* InData, const uint64_t& InSize) = 0;

	virtual void Flush() = 0;

	/**
	 * Is this interface reading ?
	 */
	virtual bool IsReading() const = 0;

	virtual bool IsBinary() const = 0;

	virtual int64_t GetSize() const = 0;
	virtual int64_t Tell() = 0;
};

/**
 * File system interface for defining a file system
 */
class ENGINECORE_API IFileSystem
{
public:
	IFileSystem(const std::string& InAlias,
		const uint8_t& InPriority) {}
	virtual ~IFileSystem() = default;

	virtual TOwnerPtr<IFile> Read(const std::string_view& InPath, const bool& bIsBinary) = 0;
	virtual TOwnerPtr<IFile> Write(const std::string_view& InPath, const bool& bIsBinary) = 0;
	virtual bool IterateDirectories(const std::string_view& InPath, 
		const TDirectoryIterator& InIt) = 0;
	virtual bool Exists(const std::string_view& InPath) = 0;
	virtual bool IsDirectory(const std::string_view& InPath) = 0;
	virtual bool IsReadOnly() const = 0;
};

}