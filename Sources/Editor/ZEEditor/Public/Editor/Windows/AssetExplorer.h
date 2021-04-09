#pragma once

#include "EngineCore.h"
#include "Window.h"
#include <filesystem>
#include "AssetDatabase/AssetDatabase.h"
#include <variant>

namespace ze::editor
{

class AssetExplorer : public Window
{
public:
	struct DirectoryEntry 
	{
		std::filesystem::path full_path;
	};
	
	AssetExplorer();
	void draw() override;
private:
	void draw_project_hierarchy();
	void draw_project_hierarchy_tree(const std::filesystem::path& in_root);
	void draw_asset_list();
	void draw_asset_entry(const std::string& in_name, 
		const std::variant<assetdatabase::AssetPrimitiveData, DirectoryEntry>& in_data);

	void set_current_path(const std::filesystem::path& in_current_path);
private:
	void draw_asset_entry();
private:
	std::filesystem::path current_path;
};

}