#include "Editor/Windows/Tools.h"
#include "ImGui/ImGui.h"
#include <imgui_internal.h>

namespace ze::editor
{

Tools::Tools() : Window("Tools", WindowFlags(), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration) 
{
	window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
}

void Tools::draw()
{
	ImGui::SetCursorPosY(20.f);
	ImGui::Separator();
	for(size_t i = 0; i < 8; ++i)
	{
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 64/2);
		ImGui::Button("A", ImVec2(64, 64));
	}
}

}