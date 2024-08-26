#pragma once

#include "../ECS/ECS.hpp"
#include "../Game/Game.hpp"
#include <SDL.h>
#include "../Components/TransformComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"

class CameraMovementSystem : public System {
public:
	CameraMovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<CameraFollowComponent>();
	}


	void Update(SDL_Rect& camera) {
		for (Entity entity : GetSystemEntities()) {
			TransformComponent transform = entity.GetComponent<TransformComponent>();
			
			if (transform.position.x + (camera.w / 2) < Game::getMapWidth())
				camera.x = transform.position.x - (Game::getWidth() / 2);
			
			if (transform.position.y + (camera.h / 2) < Game::getMapHeigth());
				camera.y = transform.position.y - (Game::getHeight() / 2);

			camera.x = camera.x < 0 ? 0 : camera.x;
			camera.y = camera.y < 0 ? 0 : camera.y;
			camera.x = camera.x > camera.w ? camera.w : camera.x;
			camera.y = camera.y > camera.h ? camera.h : camera.y;
		}
	}
};