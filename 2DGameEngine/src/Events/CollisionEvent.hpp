#pragma once

#include "../ECS/ECS.hpp"
#include "../EventBus/Event.hpp"

class CollisionEvent : public Event {
public:
	Entity a;
	Entity b;
	CollisionEvent(Entity a, Entity b): a(a), b(b){}
};