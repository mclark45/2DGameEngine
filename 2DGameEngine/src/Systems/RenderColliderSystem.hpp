#pragma once

#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include <SDL.h>

class RenderColliderSystem : public System {

public:
	RenderColliderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(SDL_Renderer* renderer, SDL_Rect& camera, bool collision) {
		for (Entity entity : GetSystemEntities()) {
			const TransformComponent transform = entity.GetComponent<TransformComponent>();
			const BoxColliderComponent collider = entity.GetComponent<BoxColliderComponent>();

			SDL_Rect colliderRect{
				static_cast<int>(transform.position.x + collider.offset.x - camera.x),
				static_cast<int>(transform.position.y + collider.offset.y - camera.y),
				static_cast<int>(collider.width * transform.scale.x),
				static_cast<int>(collider.height * transform.scale.y)
			};

			if (collision)
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			else
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(renderer, &colliderRect);
		}
	}
};