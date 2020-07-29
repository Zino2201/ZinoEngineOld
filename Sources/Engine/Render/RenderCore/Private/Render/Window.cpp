#include "Render/Window.h"
#include <SDL2/SDL.h>
#include "Module/Module.h"

namespace ZE
{

DEFINE_MODULE(ZE::Module::CDefaultModule, RenderCore);

CWindow::CWindow(const char* InName, const uint32_t& InWidth, const uint32_t& InHeight, 
	const EWindowFlags& InFlags)
	: Name(InName), Width(InWidth), Height(InHeight), Handle(nullptr)
{
	Uint32 WindowsFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

	if(InFlags & EWindowFlagBits::Resizable)
		WindowsFlags |= SDL_WINDOW_RESIZABLE;

	if (InFlags & EWindowFlagBits::Maximized)
		WindowsFlags |= SDL_WINDOW_MAXIMIZED;

	if (InFlags & EWindowFlagBits::Borderless)
		WindowsFlags |= SDL_WINDOW_BORDERLESS;

	int Pos = 0;
	if (InFlags & EWindowFlagBits::Centered)
		Pos = SDL_WINDOWPOS_CENTERED;

	Handle = SDL_CreateWindow(InName, 
		Pos,
		Pos,
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

void CWindow::SetWidth(const uint32_t& InWidth) 
{ 
	Width = InWidth; 
}

void CWindow::SetHeight(const uint32_t& InHeight) 
{ 
	Height = InHeight; 
}

} /* namespace ZE */