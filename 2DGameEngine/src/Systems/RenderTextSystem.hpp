#pragma once

#include "../ECS/ECS.hpp"
#include "../Components/TextLabelComponent.hpp"
#include "../AssetStore/AssetStore.hpp"
#include <SDL.h>

class RenderTextSystem : public System {
public:

	RenderTextSystem() {
		RequireComponent<TextLabelComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
		for (Entity entity : GetSystemEntities()) {
			const TextLabelComponent textLabel = entity.GetComponent<TextLabelComponent>();

			SDL_Surface* surface = TTF_RenderText_Blended(assetStore->GetFont(textLabel.assetId), textLabel.text.c_str(), textLabel.color);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;

			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

			SDL_Rect dst = {
				static_cast<int>(textLabel.position.x - (textLabel.isFixed ? 0 : camera.x)),
				static_cast<int>(textLabel.position.y - (textLabel.isFixed ? 0 : camera.y)),
				labelWidth,
				labelHeight
			};

			SDL_RenderCopy(renderer, texture, NULL, &dst);
			SDL_DestroyTexture(texture);
		}
	}
};