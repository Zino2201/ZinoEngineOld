#pragma once

#include <SDL2/SDL_keycode.h>

union SDL_Event;

namespace ZE::Input
{

/**
 * Clear the input system state
 */
ENGINE_API void Clear();

ENGINE_API void OnKeyPressed(const SDL_Event& InEvent);
ENGINE_API void OnKeyReleased(const SDL_Event& InEvent);
ENGINE_API bool IsKeyPressed(const SDL_Scancode& InKey);
ENGINE_API bool IsKeyReleased(const SDL_Scancode& InKey);
ENGINE_API bool IsKeyHeld(const SDL_Scancode& InKey);

}