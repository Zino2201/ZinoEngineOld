#include "ZEFS/StdFileSystem.h"
#include <fstream>

namespace ze::filesystem
{

StdFileSystem::StdFileSystem(const std::string& in_alias,
	const uint8_t& in_priority, const std::string& in_root) : FileSystem(in_alias,
		in_priority), root(in_root) {}

OwnerPtr<std::streambuf> StdFileSystem::read(const std::filesystem::path& path, const FileReadFlags& flags)
{
	std::filesystem::path correct_path = get_correct_path(path);
	std::filebuf* file = new std::filebuf;
	
	uint32_t rflags = std::ios::in;
	if (flags & FileReadFlagBits::Binary)
		rflags |= std::ios::binary;
	
	if (flags & FileReadFlagBits::End)
		rflags |= std::ios::ate;
	
	file->open(correct_path, rflags);
	if (!file->is_open())
	{
		ze::logger::error("Failed to open file {}",
			path.string());
		delete file;
		file = nullptr;
	}

	return file;
}

OwnerPtr<std::streambuf> StdFileSystem::write(const std::filesystem::path& path, const FileWriteFlags& flags)
{
	std::filesystem::path correct_path = get_correct_path(path);

	OwnerPtr<std::filebuf> file= new std::filebuf;

	uint32_t wflags = std::ios::out;
	if (flags & FileWriteFlagBits::Binary)
		wflags |= std::ios::binary;

	file->open(correct_path, wflags);
	if (!file->is_open())
	{
		ze::logger::error("Failed to open file {}",
			path.string());
		delete file;
		file = nullptr;
	}

	return file;
}

bool StdFileSystem::iterate_directories(const std::filesystem::path& path,
	const DirectoryIterator& iterator, const IterateDirectoriesFlags& flags)
{
	std::filesystem::path correct_path = get_correct_path(path);
	if (!iterator)
		return false;

	if (flags & IterateDirectoriesFlagBits::Recursive)
	{
		for (auto& entry : std::filesystem::recursive_directory_iterator(correct_path))
		{
			iterator.execute(DirectoryEntry(std::filesystem::relative(entry.path(), correct_path)));
		}
	}
	else
	{
		for (auto& Entry : std::filesystem::directory_iterator(correct_path))
		{
			iterator.execute(DirectoryEntry(std::filesystem::relative(Entry.path(), correct_path)));
		}
	}

	return true;
}

bool StdFileSystem::exists(const std::filesystem::path& path)
{
	return std::filesystem::exists(get_correct_path(path));
}

bool StdFileSystem::is_directory(const std::filesystem::path& path)
{
	return std::filesystem::is_directory(get_correct_path(path));
}

std::filesystem::path StdFileSystem::get_correct_path(const std::filesystem::path& path) const
{
	std::filesystem::path correct_path = path;
	if (correct_path.is_relative())
		correct_path = root / path;

	return correct_path;
}

}