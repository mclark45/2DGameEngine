#pragma once

#include "../ECS/ECS.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"

class DamageSystem : public System {
public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;
		Logger::Log("The Damage System received an event collision between entities " +
			std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));
		
		if (a.BelongsToGroup("projectiles") && b.HasTag("player")){
			OnProjectileHitsPlayer(a, b);
		}

		if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
			OnProjectileHitsPlayer(b, a);
		}

		if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
			OnProjectileHitsEnemy(a, b);
		}

		if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
			OnProjectileHitsEnemy(b, a);
		}

	}

	void Update() {

	}

	void OnProjectileHitsPlayer(Entity projectile, Entity player) {
		ProjectileComponent projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (!projectileComponent.isFriendly) {
			HealthComponent& health = player.GetComponent<HealthComponent>();

			health.healthPercentage -= projectileComponent.hitPercentDamage;

			if (health.healthPercentage <= 0) {
				player.Kill();
			}

			projectile.Kill();
		}
	}

	void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
		ProjectileComponent projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (projectileComponent.isFriendly) {
			HealthComponent& health = enemy.GetComponent<HealthComponent>();

			health.healthPercentage -= projectileComponent.hitPercentDamage;

			if (health.healthPercentage <= 0) {
				enemy.Kill();
			}

			projectile.Kill();
		}
	}
};