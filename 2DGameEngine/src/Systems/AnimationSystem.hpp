#pragma once

#include "../ECS/ECS.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include <SDL.h>

class AnimationSystem : public System {
public:
	AnimationSystem() {
		RequireComponent<SpriteComponent>();
		RequireComponent<AnimationComponent>();
	}

	void Update() {
		for (Entity entity : GetSystemEntities()) {
			AnimationComponent& animation = entity.GetComponent<AnimationComponent>();
			SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();

			animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;
			sprite.src.x = animation.currentFrame * sprite.width;

		}
	}
};