#pragma once

#include "../ECS/ECS.hpp"
#include "../Components/ProjectileComponent.hpp"

class ProjectileLifeCycleSystem : public System {
public:
	ProjectileLifeCycleSystem() {
		RequireComponent<ProjectileComponent>();
	}

	void Update() {
		for (Entity entity : GetSystemEntities()) {
			ProjectileComponent projectile = entity.GetComponent<ProjectileComponent>();

			if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
				entity.Kill();
			}
		}
	}
};