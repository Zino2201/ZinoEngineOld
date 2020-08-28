#include "Editor/AssetExplorer.h"
#include "Module/Module.h"
#include "ImGui/ImGui.h"
#include <imgui_internal.h>

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

void CAssetExplorer::DrawAssetHierarchy()
{
	ImGui::Text("y");
}

void CAssetExplorer::DrawAssetList()
{
	ImGui::Text("owi");
}

}