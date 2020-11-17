#include "Editor/Widgets/MapTabWidget.h"
#include "ImGui/ImGui.h"
#include "Editor/Widgets/ConvarViewer.h"

namespace ze::editor
{

CMapTabWidget::CMapTabWidget(World& in_world)
	: world(in_world), MapEditor(in_world)
{
}

void CMapTabWidget::Draw()
{
	{
		ui::SImGuiAutoStyleVar WinPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
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
	//static ConvarViewer viewer;
	//viewer.draw();

	ImGui::EndChild();
}

}