#include "../Logger/Logger.hpp"
#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/KeyboardControlledComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"
#include "..//Components/ProjectileEmitterComponent.hpp"
#include "..//Components/TextLabelComponent.hpp"
#include "..//Components/HealthComponent.hpp"
#include "../Systems/MovementSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/RenderColliderSystem.hpp"
#include "../Systems/DamageSystem.hpp"
#include "../Systems/KeyboardControlSystem.hpp"
#include "../Systems/CameraMovementSystem.hpp"
#include "../Systems/ProjectileEmitSystem.hpp"
#include "../Systems/ProjectileLifeCycleSystem.hpp"
#include "../Systems/RenderTextSystem.hpp"
#include "../Systems/RenderHealthBarSystem.hpp"
#include "../Systems/RenderGUISystem.hpp"
#include "Game.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <fstream>


int Game::SCREEN_WIDTH;
int Game::SCREEN_HEIGHT;
int Game::mapWidth;
int Game::mapHeight;

Game::Game(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
	Game::SCREEN_WIDTH = SCREEN_WIDTH;
	Game::SCREEN_HEIGHT = SCREEN_HEIGHT;
	Game::renderer = NULL;
	Game::window = NULL;
	camera.x = 0;
	camera.y = 0;
	camera.w = SCREEN_WIDTH;
	camera.h = SCREEN_HEIGHT;
	Game::millisecsPreviousFrame = 0;
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
	running = false;
	debugMode = false;
	Logger::Log("Game constructor called!");
}

Game::~Game() {
	Logger::Log("Game destructor called!");
}

/*
  Starts SDL and creates the SDL window and renderer
*/
void Game::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("SDL could not initialize.");
		return;
	}

	if (TTF_Init() != 0) {
		Logger::Err("SDL TTF could not initialize");
		return;
	}

	window = SDL_CreateWindow("2D Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		Logger::Err("SDL could not create window");
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		Logger::Err("SDL could not create window.");
		return;
	}

	// Initialize ImGui context
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);


	camera.x = 0;
	camera.y = 0;
	camera.w = SCREEN_WIDTH;
	camera.h = SCREEN_HEIGHT;

	running = true;
}

void Game::LoadLevel(int level) {
	// adding systems to the game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<RenderColliderSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardControlSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifeCycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<RenderHealthBarSystem>();
	registry->AddSystem<RenderGUISystem>();

	// building the asset store for the game
	assetStore->AddTexture(renderer, "tank-image", "assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "radar-image", "assets/images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "assets/tilemaps/jungle.png");
	assetStore->AddTexture(renderer, "bullet-image", "assets/images/bullet.png");
	assetStore->AddFont("charriot-font", "assets/fonts/charriot.ttf", 20);
	assetStore->AddFont("charriot-font-10", "assets/fonts/charriot.ttf", 10);

	// load tilemap
	int tileSize = 32;
	double tileScale = 2.0;
	int mapNumCols = 25;
	int mapNumRows = 20;
	std::fstream mapFile;
	mapFile.open("assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->CreateEntity();
			tile.Group("tiles");
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcX, srcY);
		}
	}

	mapFile.close();
	mapWidth = mapNumCols * tileSize * tileScale;
	mapHeight = mapNumRows * tileSize * tileScale;

	int chopperVelocity = 0;

	// creating entities and giving them components
	Entity chopper = registry->CreateEntity();
	chopper.Tag("player");
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(chopperVelocity, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1, false, 0, 32);
	chopper.AddComponent<AnimationComponent>(2, 10, true);
	chopper.AddComponent<BoxColliderComponent>(32, 32);
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
	chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0, -chopperVelocity), glm::vec2(chopperVelocity, 0), glm::vec2(0, chopperVelocity), glm::vec2(-chopperVelocity, 0));
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<HealthComponent>(100);

	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(SCREEN_WIDTH - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
	radar.AddComponent<AnimationComponent>(8, 4, true);

	Entity tank = registry->CreateEntity();
	tank.Group("enemies");
	tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
	tank.AddComponent<BoxColliderComponent>(32, 32);
	tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0), 5000, 10000, 10, false);
	tank.AddComponent<HealthComponent>(100);

	Entity truck = registry->CreateEntity();
	truck.Group("enemies");
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
	truck.AddComponent<BoxColliderComponent>(32, 32);
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0), 3000, 10000, 10, false);
	truck.AddComponent<HealthComponent>(100);

	Entity label = registry->CreateEntity();
	SDL_Color green = { 0, 150, 0 };
	label.AddComponent<TextLabelComponent>(glm::vec2((SCREEN_WIDTH / 2) - 60,10), "Chopper 1.0", "charriot-font", green);
}

/*
	This function handles setting up game ECS
*/
void Game::Setup() {
	LoadLevel(1);
}

/*
  This function controls updating game elements
*/
void Game::Update() {

	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
		SDL_Delay(timeToWait);

	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

	millisecsPreviousFrame = SDL_GetTicks();

	eventBus->Reset();

	registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

	registry->Update();

	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<ProjectileLifeCycleSystem>().Update();
}

/*
	This functions handles the main game loop
*/
void Game::Run() {

	Setup();
	while (running) {
		ProcessInput();
		Update();
		Render();
	}
}

/*
 This function handles the player input
*/
void Game::ProcessInput() {

	SDL_Event event;

	while (SDL_PollEvent(&event)) {

		ImGui_ImplSDL2_ProcessEvent(&event);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX;
		int mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);


		switch (event.type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				running = false;
			}
			if (event.key.keysym.sym == SDLK_b) {
				debugMode = !debugMode;
			}
			eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);
			break;
		default:
			break;
		}
	}
}

/*
  This function controls graphics rendering
*/
void Game::Render() {
	// set renderer color
	SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
	// clear renderer
	SDL_RenderClear(renderer);

	// call system update methods for systems that need rendering
	registry->GetSystem<RenderSystem>().Update(renderer, camera, assetStore);
	registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);
	if (debugMode) {
		// show hit boxes
		registry->GetSystem<RenderColliderSystem>().Update(renderer, camera, registry->GetSystem<CollisionSystem>().GetCollided());

		registry->GetSystem<RenderGUISystem>().Update(registry, camera);
	}
	
	// update the renderer
	SDL_RenderPresent(renderer);
}

/*
	This function gets the width of the screen
	@return int SCREEN_WIDTH
*/
int Game::getWidth() {
	return SCREEN_WIDTH;
}

/*
	This function gets the height of the screen
	@return int SCREEN_HEIGHT
*/
int Game::getHeight() {
	return SCREEN_HEIGHT;
}

int Game::getMapWidth() {
	return mapWidth;
}

int Game::getMapHeigth() {
	return mapHeight;
}

/*
	This function frees the created resources
	and closes SDL
*/
void Game::Destroy() {
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	SDL_Quit();
}