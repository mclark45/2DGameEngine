#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"
#include "memory"
#include "../EventBus/EventBus.hpp"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
public:
	Game(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	~Game();
	void Initialize();
	void Run();
	void Setup();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();
	void LoadLevel(int level);

	static int getWidth();
	static int getHeight();
	static int getMapWidth();
	static int getMapHeigth();


private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Rect camera;
	int millisecsPreviousFrame;
	static int SCREEN_WIDTH;
	static int SCREEN_HEIGHT;
	static int mapWidth;
	static int mapHeight;
	bool running;
	bool debugMode;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;
};
