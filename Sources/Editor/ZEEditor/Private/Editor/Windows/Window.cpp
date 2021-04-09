#include "Editor/Windows/Window.h"
#include "Reflection/Serialization.h"

namespace ze::editor
{

static int window_internal_class_counter = 1;

Window::Window(const std::string& in_title, 
	const WindowFlags& in_flags, int in_imgui_flags) : 
	title(in_title), flags(in_flags), imgui_flags(in_imgui_flags), internal_childs_class(window_internal_class_counter++),
	next_dock_id(-2201), parent(nullptr)
{
	if(flags & WindowFlagBits::Transient)
		imgui_flags |= ImGuiWindowFlags_NoSavedSettings;
}

void Window::add(Window* in_window)
{
	childs.emplace_back(in_window);
	childs.back()->window_class.ClassId = internal_childs_class;
	childs.back()->window_class.DockingAllowUnclassed = true;
	childs.back()->set_parent(this);
}

void Window::destroy_child(Window* in_child)
{
	for(size_t i = 0; i < childs.size(); ++i)
	{
		if(childs[i].get() == in_child)
		{
			childs.erase(childs.begin() + i);
			break;
		}
	}
}

bool Window::draw_window()
{
	/**
	 * Destroy expired childs
	 */
	/*for(const auto& child : expired_childs)
		destroy_child(child);
	expired_childs.clear();*/

	ImGui::PushID(title.c_str());
	ImGui::SetNextWindowClass(&window_class);
	if(flags & WindowFlagBits::DockToMainDockSpaceOnce)
	{
		ImGui::SetNextWindowDockID(ImGuiID(2201), ImGuiCond_Once);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool keep_open = true;
	if(ImGui::Begin(title.c_str(), &keep_open, imgui_flags))
	{
		ImGui::PopStyleVar();
		
		if(flags & WindowFlagBits::HasExplicitDockSpace)
		{
			std::string dockspace("EDS_" + title);
			ImGui::DockSpace(ImGui::GetID(dockspace.c_str()), ImVec2(0, 0), ImGuiDockNodeFlags_None, &window_class);
		}
		draw();
		ImGui::End();	

		/**
		 * Draw childs
		 */
		for(const auto& child : childs)
		{
			bool expired = child->draw_window();
			if(!expired)
				expired_childs.emplace_back(child.get());
		}
	}
	else
	{
		ImGui::PopStyleVar();
		ImGui::End();
	}
	ImGui::PopID();
	
	return keep_open;

}

}