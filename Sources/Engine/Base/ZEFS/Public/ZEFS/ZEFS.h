#pragma once

#include "FileSystem.h"

/**
 * ZinoEngine FileSystem
 */
namespace ZE::FileSystem
{

/**
 * ***************************************
 *					Basics
 * ***************************************
 */

/**
 * Check if the specified file or directory exists
 */
ZEFS_API bool Exists(const std::filesystem::path& InPath);

/** Check if the specified path is a directory */
ZEFS_API bool IsDirectory(const std::filesystem::path& InPath);

/**
 * ***************************************
 *			  File manipulation
 * ***************************************
 */

/**
 * Create a file archive of the specified path for reading
 * @return Returns a stream buffer for manipulating the file
 */
ZEFS_API TOwnerPtr<std::streambuf> Read(const std::filesystem::path& InPath,
	const EFileReadFlags& InReadFlags = EFileReadFlagBits::None);

/**
 * Create a file archive of the specified path for writing
 * @return Returns a stream buffer for manipulating the file
 */
ZEFS_API TOwnerPtr<std::streambuf> Write(const std::filesystem::path& InPath,
	const EFileWriteFlags& InWriteFlags = EFileWriteFlagBits::None); 


/**
 * ***************************************
 *			Directory manipulation
 * ***************************************
 */

/**
 * Iterate over all directories
 */
ZEFS_API bool IterateDirectories(const std::filesystem::path& InPath,
	const TDirectoryIterator& InIt, const EIterateDirectoriesFlags& InFlags = EIterateDirectoriesFlagBits::None);
/**
 * ***************************************
 *					Misc
 * ***************************************
 */

/** Set the current write file system */
ZEFS_API void SetWriteFS(IFileSystem& InFS);

/** 
 * Add a file system 
 * @param InName Name of the FS. Must be unique !
 * @param Alias
 */
ZEFS_API IFileSystem& AddFileSystem(const std::string& InName, const std::string& InAlias,
	const uint8_t& InPriority, TOwnerPtr<IFileSystem> InFS);

template<typename T, typename... Args>
ZE_FORCEINLINE IFileSystem& AddFileSystem(const std::string& InName, const std::string& InAlias,
	const uint8_t& InPriority, Args&&... InArgs)
{
	return AddFileSystem(InName, InAlias, InPriority,
		new T(InAlias, InPriority, std::forward<Args>(InArgs)...));
}

}