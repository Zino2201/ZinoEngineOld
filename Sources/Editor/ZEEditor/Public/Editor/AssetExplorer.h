#pragma once

#include "EngineCore.h"
#include <filesystem>
#include "robin_hood.h"

namespace ze::assetmanager { class AssetRequestHandle; }

namespace ze::editor
{

class ZEEDITOR_API CAssetExplorer
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
	std::filesystem::path CurrentDirectory = "Assets/";
	std::filesystem::path selected_dir;
	std::vector<std::shared_ptr<assetmanager::AssetRequestHandle>> pending_requests;
};

}