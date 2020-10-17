#include "PathTree.h"
#include <sstream>
#include "StringUtil.h"

namespace ze::assetdatabase
{

void PathTree::add(const std::filesystem::path& path)
{
	ZE_CHECK(path.is_relative());

	/** Check if path already exists */
	if (has_path(path))
		return;

	/** Insert the path in the map if it is a directory */
	if (!path.has_extension())
		paths.insert({ path, PathDirectory() });

	/** Now scan each part to build a tree */
	std::vector<std::string> tokens = stringutil::split(path.string(),
		std::filesystem::path::preferred_separator);

	/** Insert at root */
	if (tokens.size() == 1)
	{
		paths[""].childs.insert(path);
	}
	else
	{
		std::filesystem::path final_path;

		std::filesystem::path parent = "";
		for (size_t i = 0; i < tokens.size() - 1; ++i)
		{
			const auto& token = tokens[i];
			final_path = final_path / token;

			if (paths[parent].childs.find(token) == paths[parent].childs.end())
			{
				paths[parent / token].parent = parent;
				paths[parent].childs.insert(token);
			}

			/** Insert file */
			if (i == tokens.size() - 2)
			{
				paths[parent / token].childs.insert(tokens[tokens.size() - 1]);
			}

			parent /= token;
		}
	}
}

std::vector<std::filesystem::path> PathTree::get_childs(const std::filesystem::path& path,
	const bool& include_files)
{
	std::vector<std::filesystem::path> childs;
	if (paths.find(path) == paths.end())
		return childs;

	childs.reserve(paths[path].childs.size());
	for (const auto& child : paths[path].childs)
	{
		if (!include_files && child.has_extension())
			continue;
		
		childs.emplace_back(child);
	}

	return childs;
}

bool PathTree::has_path(const std::filesystem::path& path) const
{
	if (path.has_filename())
	{
		/** Check to the parent of the file */
		auto parent_it = paths.find(path.parent_path());
		if (parent_it != paths.end())
		{
			return parent_it->second.childs.find(path.filename()) != parent_it->second.childs.end();
		}

		return false;
	}
	else
	{
		return paths.find(path) != paths.end();
	}
}

}