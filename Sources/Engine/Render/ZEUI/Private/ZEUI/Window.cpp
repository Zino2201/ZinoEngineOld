#include "ZEUI/Window.h"
#include <SDL2/SDL.h>
#include "ImGui/ImGui.h"
#include "imgui_internal.h"

namespace ze::ui
{

Window::Window(const uint32_t in_width,
	const uint32_t in_height,
	const std::string_view& in_title,
	const uint32_t in_x,
	const uint32_t in_y,
	const WindowFlags in_flags)
	: CompositeWidget(), width(in_width), height(in_height), title(in_title), x(in_x), y(in_y), handle(nullptr)
{
	if(in_flags & WindowFlagBits::Centered)
	{
		x = SDL_WINDOWPOS_CENTERED;
		y = SDL_WINDOWPOS_CENTERED;
	}
	
	uint32_t flags = SDL_WINDOW_VULKAN;
	if(in_flags & WindowFlagBits::Maximized)
		flags |= SDL_WINDOW_MAXIMIZED;

	if(in_flags & WindowFlagBits::Resizable)
		flags |= SDL_WINDOW_RESIZABLE;
	
	handle = SDL_CreateWindow(title.c_str(),
		x,
		y,
		width,
		height,
		flags);
	ZE_CHECK(handle);

	int w, h;
	SDL_GetWindowSize(handle, &w, &h);
	width = w;
	height = h;
}

Window::~Window()
{
	ZE_CHECK(handle);
	SDL_DestroyWindow(handle);
}

void Window::paint()
{
	ImGui::PushID(title.c_str(), title.c_str() + title.size());
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	{
		SImGuiAutoStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		SImGuiAutoStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		SImGuiAutoStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::Begin("Window", nullptr,
			ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize);
	}
	
	CompositeWidget::paint();
	ImGui::End();
	ImGui::PopID();
}

}