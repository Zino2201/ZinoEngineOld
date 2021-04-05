#include "Editor/Windows/Window.h"
#include "ImGui/ImGui.h"
#include "Reflection/Serialization.h"

namespace ze::editor
{

static int window_internal_class_counter = 1;

Window::Window(const std::string& in_title, 
	const WindowFlags& in_flags, int in_imgui_flags) : 
	title(in_title), flags(in_flags), imgui_flags(in_imgui_flags), internal_childs_class(window_internal_class_counter)
{
	
}

void Window::add(Window* in_window)
{
	childs.emplace_back(in_window);
	childs.back()->set_internal_childs_class(window_internal_class_counter);
}

void Window::draw_window()
{
	ImGuiWindowClass window_class;
	window_class.ClassId = internal_childs_class;
	window_class.ParentViewportId = ImGui::GetWindowViewport()->ID;
	window_class.DockingAllowUnclassed = false;

	ImGui::PushID(title.c_str());
	ImGui::SetNextWindowClass(&window_class);
	ImGui::Begin(title.c_str(), nullptr, imgui_flags);
	draw();
	ImGui::End();
	for(const auto& child : childs)
	{
		child->draw_window();
	}
	ImGui::PopID();
}

}