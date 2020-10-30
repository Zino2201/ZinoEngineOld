#include "Engine/NativeWindow.h"
#include <SDL.h>
#include "Module/Module.h"

namespace ze
{

NativeWindow::NativeWindow(const char* in_name, const uint32_t in_width, const uint32_t in_height, 
	const int x, const int y, const NativeWindowFlags in_flags)
	: handle(nullptr), name(in_name), width(in_width), height(in_height)
{
	Uint32 win_flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

	if(in_flags & NativeWindowFlagBits::Resizable)
		win_flags |= SDL_WINDOW_RESIZABLE;

	if (in_flags& NativeWindowFlagBits::Maximized)
		win_flags |= SDL_WINDOW_MAXIMIZED;

	if (in_flags & NativeWindowFlagBits::Borderless)
		win_flags |= SDL_WINDOW_BORDERLESS;

	handle = SDL_CreateWindow(in_name, 
		in_flags & NativeWindowFlagBits::Centered ? SDL_WINDOWPOS_CENTERED : x,
		in_flags & NativeWindowFlagBits::Centered ? SDL_WINDOWPOS_CENTERED : y,
		width, height,
		win_flags);

	/** Set the width & height because SDL_WINDOW_MAXIMIZED may changed them */
	int w, h;
	SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(handle), &w, &h);
	set_width(w);
	set_height(h);
}

NativeWindow::~NativeWindow()
{
	SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(handle));
	handle = nullptr;
}

void NativeWindow::set_width(const uint32_t& in_width) 
{ 
	width = in_width; 
}

void NativeWindow::set_height(const uint32_t& in_height) 
{ 
	height = in_height; 
}

} /* namespace ze */