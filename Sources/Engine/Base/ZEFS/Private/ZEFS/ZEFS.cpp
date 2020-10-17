#include "EngineCore.h"
#include "ZEFS/ZEFS.h"
#include <map>
#include "Module/Module.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, ZEFS);

namespace ze::filesystem
{

struct FSEntry
{
	std::string name;
	uint8_t priority;
	std::string alias;

	FSEntry() : priority(0) {}
	FSEntry(const std::string& in_name, const std::string& in_alias,
		const uint8_t& in_priority) : name(in_name), priority(in_priority), alias(in_alias) {}

	bool operator<(const FSEntry& other) const
	{
		return priority < other.priority;
	}
};

/** Sorted map of all file systems */
std::map<FSEntry, std::unique_ptr<FileSystem>> filesystems;

/** Ref to the writer file system, if specified.
 * Defaults to the first added that is not read only */
FileSystem* write_fs = nullptr;

/**
 * Get filesystems that has a matching alias inside the path
 */
std::vector<FileSystem*> get_filesystems_matching_alias(const std::filesystem::path& path)
{
	std::vector<FileSystem*> r_filesystems;
	r_filesystems.reserve(2);

	for (const auto& [entry, fs] : filesystems)
	{
		if (entry.alias == "/" ||
			path.string().rfind(entry.alias, 0) == 0)
		{
			r_filesystems.emplace_back(fs.get());
		}
	}

	return r_filesystems;
}

/**
 * Helper functions calling a lambda using filesystems
 * gathered using GetFileSystemsMatchingAlias
 */
template<typename Lambda>
bool execute(const std::filesystem::path& path, Lambda&& lambda)
{
	std::vector<FileSystem*> r_filesystems = get_filesystems_matching_alias(path);

	for (const auto& fs : r_filesystems)
	{
		if (lambda(fs))
			return true;
	}

	return false;
}

template<typename Ret, bool write = false, typename Lambda>
Ret execute_ptr(const std::filesystem::path& path, Lambda&& lambda)
{
	std::vector<FileSystem*> r_filesystems = get_filesystems_matching_alias(path);

	if constexpr(write)
	{
		if (write_fs)
			return lambda(write_fs);
	}
	else
	{
		for (const auto& fs : r_filesystems)
		{
			Ret ptr = lambda(fs);
			if (ptr)
				return ptr;
		}
	}

	return nullptr;
}

OwnerPtr<std::streambuf> read(const std::filesystem::path& path,
	const FileReadFlags& flags)
{
	return execute_ptr<OwnerPtr<std::streambuf>>(path,
		[path, flags](FileSystem* fs) -> OwnerPtr<std::streambuf>
		{
			return fs->read(path, flags);
		});
}

OwnerPtr<std::streambuf> write(const std::filesystem::path& path,
	const FileWriteFlags& flags)
{
	return execute_ptr<OwnerPtr<std::streambuf>, true>(path,
		[path, flags](FileSystem* fs) -> OwnerPtr<std::streambuf>
		{
			if (fs->exists(path))
			{
				if (!(flags & FileWriteFlagBits::ReplaceExisting))
				{
					ze::logger::error("Can't write to file {}: file already exists",
						path.string());
					return nullptr;
				}
			}

			return fs->write(path, flags);
		});
}

bool exists(const std::filesystem::path& path)
{
	return execute(path,
		[path](FileSystem* fs) -> bool
		{
			return fs->exists(path);
		});
}

bool is_directory(const std::filesystem::path& path)
{
	return execute(path,
		[path](FileSystem* fs) -> bool
		{
			return fs->is_directory(path);
		});
}

bool iterate_directories(const std::filesystem::path& path,
	const DirectoryIterator& iterator, const IterateDirectoriesFlags& flags)
{
	return execute(path,
		[path, iterator, flags](FileSystem* fs) -> bool
		{
			return fs->iterate_directories(path, iterator, flags);
		});
}

void set_write_fs(FileSystem& fs)
{
	ZE_ASSERT(!fs.is_read_only());
	
	write_fs = &fs;
}

FileSystem& add_filesystem(const std::string& name, const std::string& alias,
	const uint8_t& priority, OwnerPtr<FileSystem> fs)
{
	filesystems.insert({ FSEntry(name, alias, priority),
		std::unique_ptr<FileSystem>(fs) });

	ze::logger::info("Added new filesystem {} (alias {}/{})", 
		name,
		alias.c_str(),
		priority);

	return *fs;
}

}