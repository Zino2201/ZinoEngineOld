#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Module/Module.h"

namespace ZE
{

DEFINE_MODULE(ZE::CDefaultModule, RenderCore);

CWindow::CWindow(const char* InName, const uint32_t& InWidth, const uint32_t& InHeight, 
	const EWindowFlags& InFlags)
	: Name(InName), Width(InWidth), Height(InHeight), Handle(nullptr)
{
	Uint32 WindowsFlags = SDL_WINDOW_VULKAN;

	if(HAS_FLAG(InFlags, EWindowFlags::Resizable))
		WindowsFlags |= SDL_WINDOW_RESIZABLE;

	if (HAS_FLAG(InFlags, EWindowFlags::Maximized))
		WindowsFlags |= SDL_WINDOW_MAXIMIZED;

	if (HAS_FLAG(InFlags, EWindowFlags::Borderless))
		WindowsFlags |= SDL_WINDOW_BORDERLESS;

	Handle = SDL_CreateWindow(InName, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		InWidth, InHeight,
		WindowsFlags);

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