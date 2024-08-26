#pragma once

#include "../ECS/ECS.hpp"
#include "../EventBus/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"

class KeyboardControlSystem : public System {
public:

	KeyboardControlSystem() {
		RequireComponent<KeyboardControlledComponent>();
		RequireComponent<SpriteComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::onKeyPressed);
	}

	void onKeyPressed(KeyPressedEvent& event) {
		for (Entity entity : GetSystemEntities()) {
			const KeyboardControlledComponent keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
			SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
			RigidBodyComponent& rigidBody = entity.GetComponent<RigidBodyComponent>();

			switch (event.symbol) {
				case SDLK_w:
					rigidBody.velocity = keyboardControl.upVelocity;
					sprite.src.y = sprite.height * 0;
					break;
				case SDLK_d:
					rigidBody.velocity = keyboardControl.rightVelocity;
					sprite.src.y = sprite.height * 1;
					break;
				case SDLK_s:
					rigidBody.velocity = keyboardControl.downVelocity;
					sprite.src.y = sprite.height * 2;
					break;
				case SDLK_a:
					rigidBody.velocity = keyboardControl.leftVelocity;
					sprite.src.y = sprite.height * 3;
					break;
			}
		}
	}

	void Update() {

	}

};