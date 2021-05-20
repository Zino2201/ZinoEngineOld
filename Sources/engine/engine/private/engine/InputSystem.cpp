#include "engine/InputSystem.h"
#include <robin_hood.h>
#include <SDL.h>

namespace ze::input
{

robin_hood::unordered_map<SDL_Scancode, bool> held_keys;
robin_hood::unordered_map<SDL_Scancode, bool> pressed_keys;
robin_hood::unordered_map<SDL_Scancode, bool> released_keys;
maths::Vector2f mouse_delta;

void clear()
{
	pressed_keys.clear();
	released_keys.clear();
	mouse_delta = maths::Vector2f();
}

void on_key_pressed(const SDL_Event& event)
{
	pressed_keys[event.key.keysym.scancode] = true;
	held_keys[event.key.keysym.scancode] = true;
}

void on_key_released(const SDL_Event& event)
{
	released_keys[event.key.keysym.scancode] = true;
	held_keys[event.key.keysym.scancode] = false;
}

bool is_key_pressed(const SDL_Scancode& key)
{
	return pressed_keys[key];
}

bool is_key_released(const SDL_Scancode& key)
{
	return released_keys[key];
}

bool is_key_held(const SDL_Scancode& key)
{
	return held_keys[key];
}

void set_mouse_delta(const maths::Vector2f& in_delta)
{
	mouse_delta = in_delta;
}

const maths::Vector2f& get_mouse_delta()
{
	return mouse_delta;
}

}