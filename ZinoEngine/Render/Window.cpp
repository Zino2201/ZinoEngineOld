#include "Window.h"
#include <SDL2/SDL.h>

CWindow::CWindow(const uint32_t& InWidth, const uint32_t& InHeight,
	std::string InTitle) : Width(InWidth), Height(InHeight), Title(InTitle), Window(nullptr)
{
	Window = SDL_CreateWindow(
		Title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		Width,
		Height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if (!Window)
		LOG(ELogSeverity::Fatal, "Failed to create SDL2 window");
}

CWindow::~CWindow() 
{
	if (Window)
		SDL_DestroyWindow(Window);
}