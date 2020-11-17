#pragma once

#include <SDL_keycode.h>
#include "EngineCore.h"
#include "Maths/Vector.h"

union SDL_Event;

namespace ze::input
{

/**
 * Clear the input system state
 */
ENGINE_API void clear();

ENGINE_API void on_key_pressed(const SDL_Event& event);
ENGINE_API void on_key_released(const SDL_Event& event);
ENGINE_API bool is_key_pressed(const SDL_Scancode& key);
ENGINE_API bool is_key_released(const SDL_Scancode& key);
ENGINE_API bool is_key_held(const SDL_Scancode& key);

ENGINE_API void set_mouse_delta(const maths::Vector2f& in_delta);
ENGINE_API const maths::Vector2f& get_mouse_delta();

}