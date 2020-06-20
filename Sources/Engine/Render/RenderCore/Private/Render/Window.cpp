#include "Render/Window.h"
#include <SDL2/SDL.h>

namespace ZE
{

CWindow::CWindow(const char* InName, const uint32_t& InWidth, const uint32_t& InHeight)
	: Name(InName), Width(InWidth), Height(InHeight), Handle(nullptr)
{
	Handle = SDL_CreateWindow(InName, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		InWidth, InHeight,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

	/** Set the width & height because SDL_WINDOW_MAXIMIZED may changed them */
	int W, H;
	SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(Handle), &W, &H);
	SetWidth(W);
	SetHeight(H);
}

CWindow::~CWindow()
{
	SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(Handle));
	Handle = nullptr;
}

} /* namespace ZE */