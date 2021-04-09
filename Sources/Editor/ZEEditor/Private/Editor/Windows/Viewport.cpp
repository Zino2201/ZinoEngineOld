#include "Editor/Windows/Viewport.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

Viewport::Viewport() : Window("Viewport") 
{
	window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
}

void Viewport::draw()
{
	ImVec2 p_max = ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x,
		ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y);
	
	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(),
		p_max,
		IM_COL32_BLACK);

	ImVec2 tool_max = ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x,
		ImGui::GetCursorScreenPos().y + 20.f);

	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(),
		tool_max,
		IM_COL32(70, 70, 70, 255));

	std::string str = std::to_string((int) ImGui::GetWindowSize().x) + "x" + std::to_string((int) ImGui::GetWindowSize().y);
	ImVec2 size = ImGui::CalcTextSize(str.c_str());
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - size.x - 5);
	ImGui::SetCursorPosY(ImGui::GetWindowSize().y - size.y - 5);
	ImGui::TextUnformatted(str.c_str());
}

}