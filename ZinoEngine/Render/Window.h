#pragma once

#include "Core/EngineCore.h"

struct SDL_Window;

/**
 * A SDL2 window
 */
class CWindow
{
public:
	CWindow(const uint32_t& InWidth, const uint32_t& InHeight,
		std::string InTitle = "Window");
	~CWindow();

	SDL_Window* GetSDLWindow() const { return Window; }
	const uint32_t& GetWidth() const { return Width; }
	const uint32_t& GetHeight() const { return Height; }
private:
	uint32_t Width;
	uint32_t Height;
	std::string Title;
	SDL_Window* Window;
};