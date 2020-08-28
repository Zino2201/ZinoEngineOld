#include "Editor/Widgets/MapTabWidget.h"
#include "ImGui/ImGui.h"

namespace ZE::Editor
{

void CMapTabWidget::Draw()
{
	{
		UI::SImGuiAutoStyleVar WinPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		if (!ImGui::BeginChild("TabWindowMain", ImVec2(0, 0), false, ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar))
		{
			ImGui::EndChild();
			return;
		}
	}

	ImGuiID DockspaceID = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(DockspaceID);

	/** Menu bar */
	{
		ImGui::BeginMenuBar();
		ImGui::MenuItem("File");
		ImGui::MenuItem("Edit");
		ImGui::EndMenuBar();
	}

	Console.Draw();
	MapEditor.Draw(DockspaceID);
	AssetExplorer.Draw();

	ImGui::EndChild();
}

}