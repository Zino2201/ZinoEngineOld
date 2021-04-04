#include "Editor/Windows/AssetExplorer.h"
#include "ImGui/ImGui.h"
#include <imgui_internal.h>
#include "Gfx/Gfx.h"
#include "AssetDatabase/AssetDatabase.h"

namespace ze::editor
{

static constexpr int max_hierarchy_width = 250.f;
static const ImVec2 entry_size = ImVec2(90, 120);

AssetExplorer::AssetExplorer() : Window("Asset Explorer", 
	WindowFlags(),
	ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse) 
{

}

void AssetExplorer::draw()
{
	draw_project_hierarchy();
	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();
	draw_asset_list();
}

void AssetExplorer::draw_project_hierarchy()
{
	ImGui::BeginChild("Project Hierarchy", ImVec2(max_hierarchy_width, ImGui::GetContentRegionAvail().y));
	if(ImGui::TreeNodeEx("Game", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		draw_project_hierarchy_tree("");
		ImGui::TreePop();
	}
	ImGui::EndChild();
}

void AssetExplorer::draw_project_hierarchy_tree(const std::filesystem::path& in_root)
{
	for(const auto& entry : assetdatabase::get_subdirectories(in_root))
	{
		if(ImGui::TreeNodeEx(entry.string().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			draw_project_hierarchy_tree(in_root / entry);
			ImGui::TreePop();
		}
	}
}

void AssetExplorer::draw_asset_list()
{
	ImGui::BeginChild("Asset List", ImGui::GetContentRegionAvail());
	ImGui::Columns(ImGui::GetContentRegionAvail().x / entry_size.x, nullptr, false);
	for(const auto& entry : assetdatabase::get_assets("Assets"))
	{
		/** Asset button & image */
		{
			ImVec2 cursor = ImGui::GetCursorPos();

			ImGui::Selectable(std::string("##" + entry.name).c_str(), false, ImGuiSelectableFlags_None, entry_size);
			ImGui::SetCursorPos(cursor);
			
			ImVec2 thumbnail_size = ImVec2(64, 64);
			ImVec2 cursor_pos_offset = ImVec2(thumbnail_size.x / 8, 0);
			ImRect bb(ImGui::GetCursorScreenPos() + cursor_pos_offset, ImGui::GetCursorScreenPos() + cursor_pos_offset + thumbnail_size);
		/*	ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max,
				IM_COL32(255, 0, 0, 255));*/
			ImGui::GetWindowDrawList()->AddImage(0, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), ImVec2(0, 0), ImVec2(1, 1), 
				IM_COL32_WHITE);
		}

		/** Asset name text */
		{
			std::string name = entry.name.c_str();
			ImVec2 available_size = ImGui::GetContentRegionAvail();
			ImVec2 text_size = ImGui::CalcTextSize(name.c_str());
			ImGui::SetCursorPosY(entry_size.y - text_size.y);
			ImGui::TextUnformatted(name.c_str());
		}

		ImGui::NextColumn();
	}
	ImGui::Columns(1);
	ImGui::EndChild();
}

}