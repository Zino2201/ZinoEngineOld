#pragma once

#include "EngineCore.h"
#include <filesystem>
#include <robin_hood.h>
#include <unordered_set>

namespace std
{
	template<> struct hash<std::filesystem::path>
	{
		std::size_t operator()(const std::filesystem::path& InPath) const noexcept
		{
			return std::filesystem::hash_value(InPath);
		}
	};
}

namespace ze::assetdatabase
{

/**
 * A path tree
 */
class PathTree
{
	struct PathDirectory
	{
		std::filesystem::path parent;
		std::unordered_set<std::filesystem::path> childs;
	};
public:
	/** Add the specified path to the path tree (path must be relative) */
	void add(const std::filesystem::path& path);

	std::vector<std::filesystem::path> get_childs(const std::filesystem::path& path,
		const bool& include_files);

	bool has_path(const std::filesystem::path& path) const;
private:
	std::unordered_map<std::filesystem::path, PathDirectory> paths;
};

}