#pragma once

#include "EngineCore.h"
#include "NonCopyable.h"
#include "FileSystem.h"
#include <map>

namespace ZE::FileSystem
{

/** Forward decls */
class IFileSystem;
class IFile;

/** FLAGS */

/**
 * File open flags
 */
enum class EFileReadFlagBits
{
	None = 0,
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

#pragma region API

/**
 * Create a file archive of the specified path for reading
 */
ENGINECORE_API TOwnerPtr<IFile> Read(const std::string_view& InPath,
	const EFileReadFlags& InReadFlags = EFileReadFlagBits::None);

/**
 * Create a file archive of the specified path for writing
 */
ENGINECORE_API TOwnerPtr<IFile> Write(const std::string_view& InPath, 
	const EFileWriteFlags& InWriteFlags = EFileWriteFlagBits::None); 

/**
 * Check if the specified file or directory exists
 */
ENGINECORE_API bool Exists(const std::string_view& InPath);

/**
 * Iterate over all directories
 */
ENGINECORE_API bool IterateDirectories(const std::string_view& InPath,
	const TDirectoryIterator& InIt);

/**
 * Set the current write filesystem
 */
ENGINECORE_API void SetWriteFS(IFileSystem* InFS);

/**
 * Get a filesystem by name
 */
ENGINECORE_API IFileSystem* GetFileSystemByName(const std::string_view& InName);

#pragma endregion

/**
 * File system manager singleton
 */
class ENGINECORE_API CFileSystemManager final : public CNonCopyable
{
public:
	struct SFSEntry
	{
		std::string Name;
		uint8_t Priority;
		std::string Alias;

		SFSEntry() : Priority(0), Alias() {}
		SFSEntry(const std::string& InName, const std::string& InAlias,
			const uint8_t& InPriority) : Name(InName), Priority(InPriority), Alias(InAlias) {}

		bool operator<(const SFSEntry& InOther) const
		{
			return Priority < InOther.Priority;
		}
	};

	static CFileSystemManager& Get()
	{
		static CFileSystemManager Instance;
		return Instance;
	}

	template<typename T, typename... Args>
	IFileSystem* AddFileSystem(const std::string& InName, const std::string& InAlias, 
		const uint8_t& InPriority, Args&&... InArgs)
	{
		TOwnerPtr<T> FS = new T(InAlias, InPriority, std::forward<Args>(InArgs)...);
		Filesystems.insert({ SFSEntry(InName, InAlias, InPriority),
			std::unique_ptr<T>(FS) });

		ZE::Logger::Info("Added new filesystem: alias {} ({})", InAlias.c_str(),
			InPriority);

		return FS;
	}

	void SetWriteFS(IFileSystem* InNew) { WriteFS = InNew; }

	FORCEINLINE const auto& GetFilesystems() const { return Filesystems; }
	FORCEINLINE IFileSystem* GetWriteFS() const { return WriteFS; }
private:
	CFileSystemManager() : WriteFS(nullptr) {}
public:
	CFileSystemManager(const CFileSystemManager&) = delete;
	void operator=(const CFileSystemManager&) = delete;
private:
	/** Sorted map of all file systems */
	std::map<SFSEntry, std::unique_ptr<IFileSystem>> Filesystems;

	/** Pointer to the writer file system, if specified. 
	 * Defaults to the first added that is not read only */
	IFileSystem* WriteFS;
};

}