#pragma once

#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"
#include <vector>

class CollisionSystem: public System {
public:
	CollisionSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
		collided = false;
	}

	bool GetCollided() {
		return collided;
	}

	void Update(std::unique_ptr<EventBus>& eventBus) {

		std::vector<Entity> entities = GetSystemEntities();

		// loop through entities system is interested in
		for (std::vector<Entity>::iterator i = entities.begin(); i != entities.end(); i ++) {
			Entity a = *i;

			TransformComponent aTransform = a.GetComponent<TransformComponent>();
			BoxColliderComponent aCollider = a.GetComponent<BoxColliderComponent>();

			for (std::vector<Entity>::iterator j = i; j != entities.end(); j++) {
				Entity b = *j;

				if (a == b) {
					continue;
				}

				TransformComponent bTransform = b.GetComponent<TransformComponent>();
				BoxColliderComponent bCollider = b.GetComponent<BoxColliderComponent>();

				collided = CheckAABBCollision(
					aTransform.position.x + aCollider.offset.x,
					aTransform.position.y + aCollider.offset.y,
					aCollider.width,
					aCollider.height,
					bTransform.position.x + bCollider.offset.x,
					bTransform.position.y + bCollider.offset.y,
					bCollider.width,
					bCollider.height
			    );

				if (collided) {
					Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding with " + std::to_string(b.GetId()));
					eventBus->EmitEvent<CollisionEvent>(a, b);
				}
			}
		}
	}

	bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
		return (
			aX < bX + bW &&
			aX + aW > bX &&
			aY < bY + bH &&
			aY + aH > bY
			);
	}

private:
	bool collided;
};