#pragma once

#include "EngineCore.h"

namespace ze::editor
{

enum class WindowFlagBits
{

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

	void set_internal_childs_class(int in_internal_childs_class) { internal_childs_class = in_internal_childs_class; }
protected:
	virtual void draw() = 0;
private:
	std::string title;
	WindowFlags flags;
	int imgui_flags;
	std::vector<std::unique_ptr<Window>> childs;
	int internal_childs_class;
};

}