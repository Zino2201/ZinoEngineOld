#include "ZEUI/Window.h"
#include <SDL2/SDL.h>
#include "ImGui/ImGui.h"
#include "imgui_internal.h"
#include "ZEUI/Render/Renderer.h"

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

void Window::compute_desired_size(maths::Vector2f in_available_size)
{
	content_.get().compute_desired_size(in_available_size);

	desired_size = { width, height };
}

void Window::arrange_children()
{
	if(content_.is_valid())
	{
		content_.get().set_arranged_rect(WidgetRect(maths::Vector2f(), arranged_rect.absolute_position, 
			desired_size));
	}
}

void Window::paint_window(Renderer& renderer)
{
	CompositeWidget::paint(renderer, renderer.create_context());
}

}