#pragma once

#include "EngineCore.h"
#include <filesystem>

namespace ZE::Editor
{

class ASSETEXPLORER_API CAssetExplorer
{
public:
	CAssetExplorer();

	void Draw();

	std::filesystem::path& GetCurrentDirectory() { return CurrentDirectory; }
private:
	void DrawAssetHierarchy();
	void DrawAssetList();
	void SelectDirectory(const std::filesystem::path& InPath);
	void DrawRecurseHierachy(const std::filesystem::path& InPath);
private:
	float MaxHierarchyWidth;
	std::filesystem::path CurrentDirectory;
};

}