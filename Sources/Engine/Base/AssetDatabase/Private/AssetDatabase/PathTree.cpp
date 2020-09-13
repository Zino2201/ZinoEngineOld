#include "PathTree.h"
#include <sstream>

namespace ZE
{

std::vector<std::string> Split(const std::string& InString, 
	const char& InDelimiter)
{
	std::vector<std::string> Tokens;
	Tokens.reserve(5);

	std::stringstream Stream(InString);
	std::string Token;
	while (std::getline(Stream, Token, InDelimiter)) 
	{
		/** Remove tabs */
		Token.erase(std::remove(Token.begin(), Token.end(), '\t'), Token.end());
		if(!Token.empty())
			Tokens.push_back(std::move(Token));
	}

	return Tokens;
}

void CPathTree::Add(const std::filesystem::path& InPath)
{
	verify(InPath.is_relative());

	/** Check if path already exists */
	if (HasPath(InPath))
		return;

	/** Insert the path in the map if it is a directory */
	if (!InPath.has_extension())
		Paths.insert({ InPath, SPathDirectory() });

	/** Now scan each part to build a tree */
	std::vector<std::string> Tokens = Split(InPath.string(),
		std::filesystem::path::preferred_separator);

	/** Insert at root */
	if (Tokens.size() == 1)
	{
		Paths[""].Childs.insert(InPath);
	}
	else
	{
		std::filesystem::path FinalPath;

		std::filesystem::path Parent = "";
		for (size_t i = 0; i < Tokens.size() - 1; ++i)
		{
			const auto& Token = Tokens[i];
			FinalPath = FinalPath / Token;

			if (Paths[Parent].Childs.find(Token) == Paths[Parent].Childs.end())
			{
				Paths[Parent / Token].Parent = Parent;
				Paths[Parent].Childs.insert(Token);
			}

			/** Insert file */
			if (i == Tokens.size() - 2)
			{
				Paths[Parent / Token].Childs.insert(Tokens[Tokens.size() - 1]);
			}

			Parent /= Token;
		}
	}
}

std::vector<std::filesystem::path> CPathTree::GetChilds(const std::filesystem::path& InPath,
	const bool& bInIncludeFiles)
{
	std::vector<std::filesystem::path> Childs;
	if (Paths.find(InPath) == Paths.end())
		return Childs;

	Childs.reserve(Paths[InPath].Childs.size());
	for (const auto& Child : Paths[InPath].Childs)
	{
		if (!bInIncludeFiles && Child.has_extension())
			continue;
		
		Childs.emplace_back(Child);
	}

	return Childs;
}

bool CPathTree::HasPath(const std::filesystem::path& InPath) const
{
	if (InPath.has_filename())
	{
		/** Check to the parent of the file */
		auto ParentIt = Paths.find(InPath.parent_path());
		if (ParentIt != Paths.end())
		{
			return ParentIt->second.Childs.find(InPath.filename()) != ParentIt->second.Childs.end();
		}

		return false;
	}
	else
	{
		return Paths.find(InPath) != Paths.end();
	}
}

}