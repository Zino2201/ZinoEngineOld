#include "Engine/InputSystem.h"
#include <robin_hood.h>
#include <SDL2/SDL.h>

namespace ZE::Input
{

robin_hood::unordered_map<SDL_Scancode, bool> HeldKeys;
robin_hood::unordered_map<SDL_Scancode, bool> PressedKeys;
robin_hood::unordered_map<SDL_Scancode, bool> ReleasedKeys;

void Clear()
{
	PressedKeys.clear();
	ReleasedKeys.clear();
}

void OnKeyPressed(const SDL_Event& InEvent)
{
	PressedKeys[InEvent.key.keysym.scancode] = true;
	HeldKeys[InEvent.key.keysym.scancode] = true;
}

void OnKeyReleased(const SDL_Event& InEvent)
{
	ReleasedKeys[InEvent.key.keysym.scancode] = true;
	HeldKeys[InEvent.key.keysym.scancode] = false;
}

bool IsKeyPressed(const SDL_Scancode& InKey)
{
	return PressedKeys[InKey];
}

bool IsKeyReleased(const SDL_Scancode& InKey)
{
	return ReleasedKeys[InKey];
}

bool IsKeyHeld(const SDL_Scancode& InKey)
{
	return HeldKeys[InKey];
}

}