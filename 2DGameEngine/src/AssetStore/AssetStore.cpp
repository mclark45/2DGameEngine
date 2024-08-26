#include "AssetStore.hpp"
#include "../Logger/Logger.hpp"
#include <SDL.h>
#include <SDL_image.h>

AssetStore::AssetStore() {
	Logger::Log("AssetStore constructor called!");
}

AssetStore::~AssetStore() {
	ClearAssets();
	Logger::Log("AssetStore deconstructor called!");
}

void AssetStore::ClearAssets() {
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();

	for (auto font : fonts) {
		TTF_CloseFont(font.second);
	}
	fonts.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath) {
	SDL_Surface* surface = IMG_Load(filepath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	textures.emplace(assetId, texture);

	Logger::Log("New texture added to Asset Store with id = " + assetId);
	
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
	return textures[assetId];
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filePath, int fontSize) {
	fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetStore::GetFont(const std::string& assetId) {
	return fonts[assetId];
}
