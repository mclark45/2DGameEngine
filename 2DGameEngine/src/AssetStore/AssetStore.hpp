#pragma once

#include <map>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class AssetStore {
public:
	AssetStore();
	~AssetStore();

	void ClearAssets();

	void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath);
	SDL_Texture* GetTexture(const std::string& assetId);

	void AddFont(const std::string& assetId, const std::string& filePath, int fontSize);
	TTF_Font* GetFont(const std::string& assetId);


private:
	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, TTF_Font*> fonts;
};