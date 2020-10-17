#pragma once

#include "FileSystem.h"

/**
 * ZinoEngine FileSystem
 */
namespace ze::filesystem
{

/**
 * ***************************************
 *					Basics
 * ***************************************
 */

/**
 * Check if the specified file or directory exists
 */
ZEFS_API bool exists(const std::filesystem::path& path);

/** Check if the specified path is a directory */
ZEFS_API bool is_directory(const std::filesystem::path& path);

/**
 * ***************************************
 *			  File manipulation
 * ***************************************
 */

/**
 * Create a file archive of the specified path for reading
 * @return Returns a stream buffer for manipulating the file
 */
ZEFS_API OwnerPtr<std::streambuf> read(const std::filesystem::path& path,
	const FileReadFlags& flags = FileReadFlagBits::None);

/**
 * Create a file archive of the specified path for writing
 * @return Returns a stream buffer for manipulating the file
 */
ZEFS_API OwnerPtr<std::streambuf> write(const std::filesystem::path& path,
	const FileWriteFlags& flags = FileWriteFlagBits::None); 


/**
 * ***************************************
 *			Directory manipulation
 * ***************************************
 */

/**
 * Iterate over all directories
 */
ZEFS_API bool iterate_directories(const std::filesystem::path& path,
	const DirectoryIterator& iterator, const IterateDirectoriesFlags& flags = IterateDirectoriesFlagBits::None);
/**
 * ***************************************
 *					Misc
 * ***************************************
 */

/** Set the current write file system */
ZEFS_API void set_write_fs(FileSystem& fs);

/** 
 * Add a file system 
 * @param InName Name of the FS. Must be unique !
 * @param Alias
 */
ZEFS_API FileSystem& add_filesystem(const std::string& name, const std::string& alias,
	const uint8_t& priority, OwnerPtr<FileSystem> fs);

template<typename T, typename... Args>
ZE_FORCEINLINE FileSystem& add_filesystem(const std::string& name, const std::string& alias,
	const uint8_t& priority, Args&&... args)
{
	return add_filesystem(name, alias, priority,
		new T(alias, priority, std::forward<Args>(args)...));
}

}