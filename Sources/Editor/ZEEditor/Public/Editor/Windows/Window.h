#pragma once

#include "EngineCore.h"
#include "ImGui/ImGui.h"

namespace ze::editor
{

enum class WindowFlagBits
{
	/** Does the window contains a dock space ? Allows childrens to dock inside the window */
	HasExplicitDockSpace = 1 << 0,

	/** Should ImGui save data about this window in the .ini ? */
	Transient = 1 << 1,

	/** Automaticly dock to the main dockspace once */
	DockToMainDockSpaceOnce = 1 << 2
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
	virtual ~Window() = default;

	/**
	 * Draw the window and its childs
	 * \return True if the window is still opened
	 */
	bool draw_window();

	/** 
	 * Add the specified window as a child of the window
	 */
	void add(Window* in_window);

	/**
	 * Set new parent
	 */
	void set_parent(Window* in_parent) { parent = in_parent; }

	/** 
	 * Destroy the specified child window
	 */
	void destroy_child(Window* in_child);

	ZE_FORCEINLINE const std::string& get_title() const { return title; }
protected:
	virtual void draw() = 0;
protected:
	std::string title;
	WindowFlags flags;
	int imgui_flags;
	std::vector<std::unique_ptr<Window>> childs;
	int internal_childs_class;
	ImGuiWindowClass window_class;
	ImGuiID next_dock_id;
	Window* parent;
};

}