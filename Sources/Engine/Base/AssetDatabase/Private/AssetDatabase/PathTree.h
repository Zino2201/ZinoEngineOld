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

namespace ZE
{

/**
 * A path tree
 */
class CPathTree
{
	struct SPathDirectory
	{
		std::filesystem::path Parent;
		std::unordered_set<std::filesystem::path> Childs;
	};
public:
	/** Add the specified path to the path tree (path must be relative) */
	void Add(const std::filesystem::path& InPath);

	std::vector<std::filesystem::path> GetChilds(const std::filesystem::path& InPath,
		const bool& bInIncludeFiles);

	bool HasPath(const std::filesystem::path& InPath) const;
private:
	std::unordered_map<std::filesystem::path, SPathDirectory> Paths;
};

}