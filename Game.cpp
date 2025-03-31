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

std::chrono::steady_clock::time_point lastSpawnTime;
int spawnCooldown = 2000;
Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
AssetManager* Game::assets = new AssetManager(&manager);

SDL_Rect Game::camera = { 0,0,800,640 };

bool Game::isRunning = false;

auto& player(manager.addEntity());

Game::Game()
{
}

Game::~Game()
{
}
const int enemySpawnPositions[][2] = {
	{1600, 200},  {1600, 1200}
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
	assets->AddTexture("terrain_tiles", "Assets/terrain_ss.png");
	assets->AddTexture("player", "Assets/player_anims.png");
	assets->AddTexture("projectile", "Assets/ColTex.png");
	assets->AddTexture("enemy_slow", "Assets/enemy2.png");
	assets->AddTexture("enemy_fast", "Assets/enemy1.png");

	map = new Map("terrain_tiles", 3, 32);
	//ecs implementation

	map->LoadMap("assets/map.map", 25, 20);

	player.addComponent<TransformComponent>(140, 1200, 32, 32, 3);
	player.addComponent<SpriteComponent>("player", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addComponent<HealthComponent>(100);
	player.addGroup(groupPlayers);


;
}
void Game::spawnEnemy(std::string type, int x, int y) {
	
	auto& enemy = manager.addEntity();
	enemy.addComponent<TransformComponent>(x, y, 32, 32, 1);

	int speed;  
	if (type == "fast") {
		enemy.addComponent<SpriteComponent>("enemy_fast",true);
		enemy.addComponent<HealthComponent>(100);

		speed = 3;
	}
	else {
		enemy.addComponent<SpriteComponent>("enemy_slow",true);
		enemy.addComponent<HealthComponent>(200);
		speed = 1;
	}

	enemy.addGroup(Game::groupEnemies);
	enemy.addComponent<ColliderComponent>("enemy");
	
	enemy.getComponent<TransformComponent>().velocity.x = speed;
	enemy.getComponent<TransformComponent>().velocity.y = speed;	

}
	


auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& projectiles(manager.getGroup(Game::groupProjectiles));
auto& enemies(manager.getGroup(Game::groupEnemies));

void Game::handleEvents()
{

	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}


void Game::update()
{

	SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player.getComponent<TransformComponent>().position;

	manager.refresh();
	manager.update();

	for (auto& c : colliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			player.getComponent<TransformComponent>().position = playerPos;
		}
	}

	for (auto& p : projectiles)
	{
		if (Collision::AABB(playerCol, p->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Player Hit" << std::endl;
			
		}
		
	}
	for (auto& e : enemies) {
		auto& playerTransform = player.getComponent<TransformComponent>();
		auto& enemyTransform = e->getComponent<TransformComponent>();

		float dx = playerTransform.position.x - enemyTransform.position.x;
		float dy = playerTransform.position.y - enemyTransform.position.y;
		float distance = std::sqrt(dx * dx + dy * dy);

		 
		

		if (distance > 1) {
			enemyTransform.velocity.x = (dx / distance)/2 ;
			enemyTransform.velocity.y = (dy / distance)/2;
		}
		for (auto& p : projectiles)
		{
			if (Collision::AABB(p->getComponent<ColliderComponent>().collider, e->getComponent<ColliderComponent>().collider))
			{
				std::cout << "Enemy Hit" << std::endl;
				e->getComponent<HealthComponent>().takeDamage(50,false);
				p->destroy();
			}
		}
		if (Collision::AABB(playerCol, e->getComponent<ColliderComponent>().collider))
		{
			std::cout << "Player Hit" << std::endl;
			player.getComponent<HealthComponent>().takeDamage(10, true);
			e->destroy();
		}
	}
	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSpawnTime).count();

	if (elapsed >= spawnCooldown) {
		int spawnIndex = rand() % 2;  // ✅ Choose a random spawn point

		// ✅ Ensure equal chance for "fast" and "slow" enemies
		std::string enemyType = (rand() % 2 == 0) ? "slow" : "fast";

		spawnEnemy(enemyType, enemySpawnPositions[spawnIndex][0], enemySpawnPositions[spawnIndex][1]);
		lastSpawnTime = std::chrono::steady_clock::now();  // Reset timer
	}
	
	camera.x = player.getComponent<TransformComponent>().position.x - 400;
	camera.y = player.getComponent<TransformComponent>().position.y - 320;

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
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}