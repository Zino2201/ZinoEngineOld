#include "Editor/Windows/Window.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

Window::Window(const std::string& in_title, 
	const WindowFlags& in_flags, int in_imgui_flags) : title(in_title), flags(in_flags), imgui_flags(in_imgui_flags)
{

}

void Window::add(Window* in_window)
{
	childs.emplace_back(in_window);
}

void Window::draw_window()
{
	ImGui::PushID(title.c_str());
	ImGui::Begin(title.c_str(), nullptr, imgui_flags);
	draw();
	ImGui::End();
	for(const auto& child : childs)
		child->draw_window();
	ImGui::PopID();
}

}