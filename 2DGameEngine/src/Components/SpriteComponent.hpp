#pragma once

#include <string>
#include <SDL.h>

struct SpriteComponent {
	std::string assetId;
	int width;
	int height;
	int zIndex;
	bool isFixed;
	SDL_Rect src;

	SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, int srcX = 0, int srcY = 0) {
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->zIndex = zIndex;
		this->isFixed = isFixed;
		this->src = { srcX, srcY, width, height };
	}
};