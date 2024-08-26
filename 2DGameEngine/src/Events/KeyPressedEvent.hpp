#pragma once

#include "../ECS/ECS.hpp"
#include "../EventBus/Event.hpp"
#include <SDL.h>

class KeyPressedEvent : public Event {
public:
	SDL_Keycode symbol;
	KeyPressedEvent(SDL_Keycode symbol): symbol(symbol){}
};