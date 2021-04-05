#pragma once

#include "EngineCore.h"
#include "Window.h"
#include <filesystem>

namespace ze::editor
{

class AssetExplorer : public Window
{
public:
	AssetExplorer();
	void draw() override;
private:
	void draw_project_hierarchy();
	void draw_project_hierarchy_tree(const std::filesystem::path& in_root);
	void draw_asset_list();
	void set_current_path(const std::filesystem::path& in_current_path);
private:
	void draw_asset_entry();
private:
	std::filesystem::path current_path;
};

}