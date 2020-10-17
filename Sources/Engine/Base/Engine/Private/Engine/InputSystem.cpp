#include "Engine/InputSystem.h"
#include <robin_hood.h>
#include <SDL.h>

namespace ze::input
{

robin_hood::unordered_map<SDL_Scancode, bool> held_keys;
robin_hood::unordered_map<SDL_Scancode, bool> pressed_keys;
robin_hood::unordered_map<SDL_Scancode, bool> released_keys;

void clear()
{
	pressed_keys.clear();
	released_keys.clear();
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

}