#include "editor/windows/Window.h"
#include "reflection/Serialization.h"
#include "engine/InputSystem.h"

namespace ze::editor
{

static int window_internal_class_counter = 1;

Window::Window(const std::string& in_title, 
	const WindowFlags& in_flags, int in_imgui_flags) : 
	title(in_title), flags(in_flags), imgui_flags(in_imgui_flags), internal_childs_class(window_internal_class_counter++),
	next_dock_id(-2201), parent(nullptr), need_save(false)
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

void Window::pre_draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
}

void Window::post_draw()
{
	ImGui::PopStyleVar();
}

bool Window::draw_window()
{
	ImGui::PushID(title.c_str());
	ImGui::SetNextWindowClass(&window_class);
	if(flags & WindowFlagBits::DockToMainDockSpaceOnce)
	{
		ImGui::SetNextWindowDockID(ImGuiID(2201), ImGuiCond_Once);
	}
	
	pre_draw();
	bool keep_open = true;
	if(ImGui::Begin(title.c_str(), &keep_open, imgui_flags))
	{
		/** Detect ctrl+s */
		if(flags & WindowFlagBits::Document &&
			ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			input::is_key_held(SDL_SCANCODE_LCTRL) &&
			input::is_key_held(SDL_SCANCODE_S))
		{
			save();
			mark_as_saved();
		}
		
		if(flags & WindowFlagBits::HasExplicitDockSpace)
		{
			std::string dockspace("EDS_" + title);
			ImGui::DockSpace(ImGui::GetID(dockspace.c_str()), ImVec2(0, 0), ImGuiDockNodeFlags_None, &window_class);
		}
		draw();
		ImGui::End();
		post_draw();

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
		post_draw();
		ImGui::End();
	}
	ImGui::PopID();
	
	return keep_open;

}

void Window::mark_as_saved()
{
	if(flags & WindowFlagBits::Document)
	{
		imgui_flags &= ~ImGuiWindowFlags_UnsavedDocument;
		need_save = false;
	}
}
	
void Window::mark_as_unsaved()
{
	if(flags & WindowFlagBits::Document)
	{
		imgui_flags |= ImGuiWindowFlags_UnsavedDocument;
		need_save = true;
	}
}
	
}