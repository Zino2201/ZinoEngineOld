#pragma once

#include "EngineCore.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

enum class WindowFlagBits
{
	/** Does the window contains a dock space ? Allows childrens to dock inside the window */
	HasExplicitDockSpace = 1 << 0
};
ENABLE_FLAG_ENUMS(WindowFlagBits, WindowFlags);

/**
 * A window (e.g map editor, texture editor)
 * It can contains childs windows that will only be renderer if theirs parents is renderer
 * This is useful for creating modular editors with multiple tabs
 */
class Window
{
public:
	Window(const std::string& in_title, const WindowFlags& in_flags = WindowFlags(),
		int in_imgui_flags = 0);

	/**
	 * Draw the window and its childs
	 */
	void draw_window();

	void add(Window* in_window);
protected:
	virtual void draw() = 0;
protected:
	std::string title;
	WindowFlags flags;
	int imgui_flags;
	std::vector<std::unique_ptr<Window>> childs;
	int internal_childs_class;
	ImGuiWindowClass window_class;
};

}