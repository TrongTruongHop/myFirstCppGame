#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "HealthComponent.h"
#include <chrono> 
#include<cmath>
#include <ctime> 
#include <cstdlib>
#include "menu.h"
#include <sstream>
#include "AssetManager.h"
#include <sdl_mixer.h>
#include "audio.h"
std::chrono::steady_clock::time_point lastSpawnTime;
int spawnCooldown = 2000; 
Map* map;
Manager manager;
bool Game::isRunning = false;
bool Game::spawnTriggered = false;
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
AssetManager* Game::assets = new AssetManager(&manager);

SDL_Rect Game::camera = { 0,0,800,640 };

Menu* menu = nullptr;
TTF_Font* Game::font = nullptr;
GameState currentState = GameState::MENU;
bool playerWon = false;
Entity* player;
Entity* boss;
Entity* label;
Entity* label2;
Game::Game()
{
}

Game::~Game()
{
}
const int enemySpawnPositions[][2] = {
	{1600, 400},  {1600, 1200}
};

void Game::init(const char* title, int width, int height, bool fullscreen)
{
	srand(static_cast<unsigned>(time(0)));
	lastSpawnTime = std::chrono::steady_clock::now();
	int flags = 0;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}

		isRunning = true;
	}
	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		isRunning = false;
		return;
	}

	
}
void Game::startGame() {
	// --- Load assets once ---
	static bool assetsLoaded = false;
	if (!assetsLoaded) {
		AudioManager::Init();
		AudioManager::LoadSound("hit", "assets/enemy.wav");
		AudioManager::LoadSound("lost", "assets/lost.wav");
		AudioManager::LoadSound("win", "assets/win.wav");
		AudioManager::LoadMusic("bgm", "assets/background.mp3");

		assets->AddFont("OpenSans", "Assets/OpenSans.ttf", 16);
		assets->AddTexture("terrain_tiles", "Assets/terrain_ss.png");
		assets->AddTexture("player", "Assets/player_anims.png");
		assets->AddTexture("projectile", "Assets/ColTex.png");
		assets->AddTexture("enemy_slow", "Assets/enemy2.png");
		assets->AddTexture("enemy_fast", "Assets/enemy1.png");
		assets->AddTexture("boss", "Assets/boss.png");
		assets->AddTexture("bomb", "Assets/bomb.png");
		assets->AddTexture("startButton", "Assets/startButton.png");
		assets->AddTexture("titleText", "Assets/titleText.png");

		Game::font = TTF_OpenFont("Assets/OpenSans.ttf", 16);
		map = new Map("terrain_tiles", 3, 32);
		map->LoadMap("assets/map.map", 25, 20);

		assetsLoaded = true;
	}

	resetGame();  // build player/boss/etc
}
void Game::resetGame() {
	for (auto& e : manager.getGroup(groupEnemies)) e->destroy();
	for (auto& p : manager.getGroup(groupProjectiles)) p->destroy();
	for (auto& bo : manager.getGroup(groupBombs)) bo->destroy();
	
	manager.refresh();
	manager.update();

	spawnTriggered = false;
	playerWon = false;
	spawnCooldown = 2000;

	player = &manager.addEntity();
	boss = &manager.addEntity();
	label = &manager.addEntity();
	label2 = &manager.addEntity();

	player->addComponent<TransformComponent>(150, 1200, 32, 32, 3);
	player->addComponent<SpriteComponent>("player", true);
	player->addComponent<KeyboardController>();
	player->addComponent<ColliderComponent>("player");
	player->addComponent<HealthComponent>(10);
	player->addGroup(groupPlayers);

	SDL_Color white = { 255, 255, 255, 255 };
	label->addComponent<UILabel>(10, 10, "Player HP: 100", "OpenSans", white);
	label2->addComponent<UILabel>(10, 30, "Boss HP: 2000", "OpenSans", white);

	boss->addComponent<TransformComponent>(1120, 750, 32, 32, 5);
	boss->addComponent<SpriteComponent>("boss", true);
	boss->addComponent<HealthComponent>(2000);
	boss->addComponent<ColliderComponent>("boss");
	boss->addGroup(groupBosses);

	// Restart music
	AudioManager::PlayMusic("bgm", -1);
}

void Game::spawnEnemy(std::string type, int x, int y) {
	
	auto& enemy = manager.addEntity();
	enemy.addComponent<TransformComponent>(x, y, 32, 32, 1);

	int speed;  
	if (type == "fast") {
		enemy.addComponent<SpriteComponent>("enemy_fast",true);
		enemy.addComponent<HealthComponent>(50);

		
	}
	else {
		enemy.addComponent<SpriteComponent>("enemy_slow",true);
		enemy.addComponent<HealthComponent>(100);
		
	}

	enemy.addGroup(Game::groupEnemies);
	enemy.addComponent<ColliderComponent>("enemy");
	
		

}



auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& projectiles(manager.getGroup(Game::groupProjectiles));
auto& enemies(manager.getGroup(Game::groupEnemies));
auto& bosses(manager.getGroup(Game::groupBosses));
auto& bombs(manager.getGroup(Game::groupBombs));

void Game::handleEvents() {
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	case SDL_KEYDOWN:
		if (currentState == GameState::PLAYING) {
			switch (event.key.keysym.sym) {
			case SDLK_w:
				player->getComponent<TransformComponent>().velocity.y = -1;
				break;
			case SDLK_s:
				player->getComponent<TransformComponent>().velocity.y = 1;
				break;
			case SDLK_a:
				player->getComponent<TransformComponent>().velocity.x = -1;
				break;
			case SDLK_d:
				player->getComponent<TransformComponent>().velocity.x = 1;
				break;
			}
		}
		else if (currentState == GameState::GAME_OVER) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				isRunning = false;
				break;
			case SDLK_m:
				             // <-- reset everything
				currentState = GameState::MENU;
				break;
			}
		}
		break;
	case SDL_KEYUP:
		if (currentState == GameState::PLAYING) {
			switch (event.key.keysym.sym) {
			case SDLK_w:
			case SDLK_s:
				player->getComponent<TransformComponent>().velocity.y = 0;
				break;
			case SDLK_a:
			case SDLK_d:
				player->getComponent<TransformComponent>().velocity.x = 0;
				break;
			}
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (currentState == GameState::MENU) {
			int x = event.button.x;
			int y = event.button.y;

			// Match button rectangle used in render()
			SDL_Rect btnRect = { 300, 300, 200, 80 };
			if (x >= btnRect.x && x <= btnRect.x + btnRect.w &&
				y >= btnRect.y && y <= btnRect.y + btnRect.h) {
				// Start the game
				currentState = GameState::PLAYING;
				startGame();
			}
		}
		break;
	default:
		break;
	}

}


void Game::update()
{
	if (currentState != GameState::PLAYING) return;



	SDL_Rect playerCol = player->getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player->getComponent<TransformComponent>().position;
	
	std::stringstream ss;
	std::stringstream ss2;
	ss << "Player HP: " << player->getComponent<HealthComponent>().health;
	label->getComponent<UILabel>().SetLabelText(ss.str(), "OpenSans");
	ss2 << "Boss HP: " << boss->getComponent<HealthComponent>().health;
	label2->getComponent<UILabel>().SetLabelText(ss2.str(), "OpenSans");

	manager.refresh();
	manager.update();
	if (!spawnTriggered &&
		playerPos.x >= 150 &&
		playerPos.y <= 750) {
		spawnTriggered = true;
		lastSpawnTime = std::chrono::steady_clock::now();  // ✅ Start spawn timer
		std::cout << "Enemy spawning triggered!\n";
	}
	for (auto& c : colliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			player->getComponent<TransformComponent>().position = playerPos;
		}
	}

	for (auto& p : projectiles)
	{
		if (Collision::AABB(playerCol, p->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Player Hit" << std::endl;
			
		}
		if (Collision::AABB(boss->getComponent<ColliderComponent>().collider, p->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Boss Hit" << std::endl;
			boss->getComponent<HealthComponent>().takeDamage(50, false);
			AudioManager::PlaySound("hit", 0);
		}
		
	}
	for (auto& bo : bombs) {
		
		int explosionRadius = 150;
		for (auto& e : enemies) {
			auto& enemyTransform = e->getComponent<TransformComponent>();
			float dx = bo->getComponent<TransformComponent>().position.x - enemyTransform.position.x;
			float dy = bo->getComponent<TransformComponent>().position.y - enemyTransform.position.y;
			float distance = std::sqrt(dx * dx + dy * dy);
			if (distance < explosionRadius) {
				std::cout << "Enemy Hit" << std::endl;
				e->getComponent<HealthComponent>().takeDamage(100, false);
				AudioManager::PlaySound("hit", 0);
			
			}
		}
	}
	for (auto& e : enemies) {
		auto& playerTransform = player->getComponent<TransformComponent>();
		auto& enemyTransform = e->getComponent<TransformComponent>();

		float dx = playerTransform.position.x - enemyTransform.position.x;
		float dy = playerTransform.position.y - enemyTransform.position.y;
		float distance = std::sqrt(dx * dx + dy * dy);

		 
		

		if (distance > 1) {
			if (boss->getComponent<HealthComponent>().health < 1000) {
				enemyTransform.velocity.x = (dx / distance)/1.15;
				enemyTransform.velocity.y = (dy / distance)/1.15;
			}
			else {
				enemyTransform.velocity.x = (dx / distance) / 1.5;
				enemyTransform.velocity.y = (dy / distance) / 1.5;
			}
		}
		for (auto& p : projectiles)
		{
			if (Collision::AABB(p->getComponent<ColliderComponent>().collider, e->getComponent<ColliderComponent>().collider))
			{
				std::cout << "Enemy Hit" << std::endl;

				e->getComponent<HealthComponent>().takeDamage(50,false);

				AudioManager::PlaySound("hit", 0);	
				p->destroy();
			}
		}
		if (Collision::AABB(player->getComponent<ColliderComponent>().collider, e->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Player Hit" << std::endl;
			player->getComponent<HealthComponent>().takeDamage(10, true);
			e->destroy();
		}
	}
	for (auto&b : bosses) {
		for (auto& p : projectiles)
		{
			if (Collision::AABB(p->getComponent<ColliderComponent>().collider, b->getComponent<ColliderComponent>().collider))
			{
				std::cout << "Boss Hit" << std::endl;
				p->destroy();
			}
		}
		if (b->getComponent<HealthComponent>().health <= 1000)
		{
			spawnCooldown = 1000;
		}
		if (b->getComponent<HealthComponent>().health <= 500)
		{
			spawnCooldown = 500;
		}
	}

	if (spawnTriggered) {
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSpawnTime).count();

		if (elapsed >= spawnCooldown) {
			int spawnIndex = rand() % 2;  

			
			std::string enemyType = (rand() % 2 == 0) ? "slow" : "fast";

			spawnEnemy(enemyType, enemySpawnPositions[spawnIndex][0], enemySpawnPositions[spawnIndex][1]);
			lastSpawnTime = std::chrono::steady_clock::now();  // Reset timer
		}
	}
	if (player->getComponent<HealthComponent>().health <= 0) {
		playerWon = false;
		currentState = GameState::GAME_OVER;
		AudioManager::PlaySound("lost", 0);
		AudioManager::StopMusic();
	}

	if (boss->getComponent<HealthComponent>().health <= 0) {
		playerWon = true;
		currentState = GameState::GAME_OVER;
		AudioManager::PlaySound("win", 0);
		AudioManager::StopMusic();
	}

	camera.x = player->getComponent<TransformComponent>().position.x - 400;
	camera.y = player->getComponent<TransformComponent>().position.y - 320;

	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > camera.w)
		camera.x = camera.w;
	if (camera.y > camera.h)
		camera.y = camera.h;
	
}

void Game::render()
{
	SDL_RenderClear(renderer);
	if (currentState == GameState::MENU) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
		SDL_RenderClear(renderer);

		// Title
		SDL_Texture* title = IMG_LoadTexture(Game::renderer, "Assets/title.png");

		SDL_Rect titleRect = { 200, 100, 400, 100 };
		SDL_RenderCopy(renderer, title, nullptr, &titleRect);

		// Start button
		SDL_Texture* startBtn = IMG_LoadTexture(Game::renderer, "Assets/startButton.png");
		SDL_Rect btnRect = { 300, 300, 200, 80 };
		SDL_RenderCopy(renderer, startBtn, nullptr, &btnRect);

		SDL_RenderPresent(renderer);
		return;
	}
		for (auto& t : tiles)
		{
			t->draw();
		}

		for (auto& c : colliders)
		{
			c->draw();
		}

		for (auto& p : players)
		{
			p->draw();
		}

		for (auto& p : projectiles)
		{
			p->draw();
		}
		for (auto& e : enemies) {
			e->draw();
		}
		for (auto& b : bosses)
		{
			b->draw();
		}
		for (auto& bo : bombs)
		{
			bo->draw();
		}
		label->draw();
		label2->draw();

	if (currentState == GameState::GAME_OVER) {
		SDL_Color black = { 255, 255, 255 };
		std::string msg;
		if (playerWon) {
			msg = "You Win!";
			SDL_Surface* surface = TTF_RenderText_Solid(Game::font, msg.c_str(), black);
			SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_Rect textRect = { 350, 250, surface->w, surface->h };
			SDL_FreeSurface(surface);
			SDL_RenderCopy(renderer, text, nullptr, &textRect);
			SDL_DestroyTexture(text);
			
		}
		else {
			msg = "Game Over";
			SDL_Surface* surface = TTF_RenderText_Solid(Game::font, msg.c_str(), black);
			SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_Rect textRect = { 300, 250, surface->w, surface->h };
			SDL_FreeSurface(surface);
			SDL_RenderCopy(renderer, text, nullptr, &textRect);
			SDL_DestroyTexture(text);
			
		}
		
		SDL_Surface* subSurface = TTF_RenderText_Solid(Game::font, "Press esc to quit", black);
		SDL_Texture* subText = SDL_CreateTextureFromSurface(renderer, subSurface);
		SDL_Rect subRect = { 300, 350, subSurface->w, subSurface->h };
		SDL_FreeSurface(subSurface);
		SDL_RenderCopy(renderer, subText, nullptr, &subRect);
		SDL_DestroyTexture(subText);
		
	}
	SDL_RenderPresent(renderer);

}

void Game::clean()
{
	
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}