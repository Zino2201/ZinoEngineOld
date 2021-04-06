#include "Editor/Windows/Window.h"
#include "Reflection/Serialization.h"

namespace ze::editor
{

static int window_internal_class_counter = 1;

Window::Window(const std::string& in_title, 
	const WindowFlags& in_flags, int in_imgui_flags) : 
	title(in_title), flags(in_flags), imgui_flags(in_imgui_flags), internal_childs_class(window_internal_class_counter++)
{
	
}

void Window::add(Window* in_window)
{
	childs.emplace_back(in_window);
	childs.back()->window_class.ClassId = internal_childs_class;
	childs.back()->window_class.DockingAllowUnclassed = true;
}

void Window::draw_window()
{
	ImGui::PushID(title.c_str());
	ImGui::SetNextWindowClass(&window_class);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(title.c_str(), nullptr, imgui_flags);
	ImGui::PopStyleVar();
	if(flags & WindowFlagBits::HasExplicitDockSpace)
	{
		std::string dockspace("EDS_" + title);
		ImGui::DockSpace(ImGui::GetID(dockspace.c_str()), ImVec2(0, 0), ImGuiDockNodeFlags_None, &window_class);
	}
	draw();
	ImGui::End();
	for(const auto& child : childs)
	{
		child->draw_window();
	}
	ImGui::PopID();
}

}