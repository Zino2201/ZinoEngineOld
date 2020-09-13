#include "Editor/AssetExplorer.h"
#include "Module/Module.h"
#include "ImGui/ImGui.h"
#include <imgui_internal.h>
#include "AssetDatabase/AssetDatabase.h"
#include "Reflection/Class.h"

DEFINE_MODULE(ZE::Module::CDefaultModule, AssetExplorer);

namespace ZE::Editor
{

CAssetExplorer::CAssetExplorer() : MaxHierarchyWidth(250.f) {}

void CAssetExplorer::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.5f, 0.5f));
	if (!ImGui::Begin("Asset Explorer"))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}
	ImGui::PopStyleVar();
	 

	/** Project Hierarchy */
	if(ImGui::BeginChild("Project Hierarchy", ImVec2(MaxHierarchyWidth, 
		ImGui::GetContentRegionAvail().y)))
		DrawAssetHierarchy();

	ImGui::EndChild();

	ImGui::SameLine();

	/** Asset list */
	if (ImGui::BeginChild("Asset List", ImGui::GetContentRegionAvail()))
		DrawAssetList();

	ImGui::EndChild();

	ImGui::End();
}

void CAssetExplorer::SelectDirectory(const std::filesystem::path& InPath)
{
	CurrentDirectory = InPath;
}

void CAssetExplorer::DrawRecurseHierachy(const std::filesystem::path& InPath)
{
	std::vector<std::filesystem::path> Subdirs = ZE::AssetDatabase::GetSubDirectories(InPath);
	
	for (const auto& Subdir : Subdirs)
	{
		if (ImGui::TreeNodeEx(Subdir.string().c_str(), ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			if (ImGui::IsItemClicked())
			{
				SelectDirectory(InPath / Subdir);
			}

			DrawRecurseHierachy(InPath / Subdir);
			ImGui::TreePop();
		}
	}
}

void CAssetExplorer::DrawAssetHierarchy()
{
	if (ImGui::TreeNodeEx("Game", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::IsItemClicked())
		{
			SelectDirectory("");
		}

		DrawRecurseHierachy("");
		ImGui::TreePop();
	}
}

void CAssetExplorer::DrawAssetList()
{
	for (const auto& Asset : ZE::AssetDatabase::GetAssets(CurrentDirectory))
	{
		if (ImGui::BeginChild(Asset.Name.c_str(), ImVec2(75, 100), true, ImGuiWindowFlags_NoScrollbar | 
			ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::Image(0, ImVec2(50, 50));
			ImGui::TextWrapped(Asset.Name.c_str());
		}
		ImGui::EndChild();
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Type: %s\nFull path: %s\nSize: %f Mb", 
				Asset.Class->GetName(),
				Asset.Path.string().c_str(),
				static_cast<float>(Asset.Size / 1024.f / 1024.f));
			ImGui::Separator();
			ImGui::Text("Editor data");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
	}
}

}