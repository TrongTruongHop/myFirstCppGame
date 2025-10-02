#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include<SDL_ttf.h>

class AssetManager;
class ColliderComponent;

class Game
{
public:
	Game();
	~Game();


	void init(const char* title, int width, int height, bool fullscreen);
	void spawnEnemy(std::string type, int x, int y);
	void handleEvents();
	void update();
	bool running() { return isRunning; }
	void render();
	void clean();

	void startGame();
	void resetGame();
	static void renderMenu();
	static void handleMenuEvent(SDL_Event& e);
	static SDL_Renderer* renderer;
	static SDL_Event event;
	static bool isRunning;
	static SDL_Rect camera;
	static bool spawnTriggered;
	static AssetManager* assets;

	enum groupLabels : std::size_t
	{
		groupMap,
		groupPlayers,
		groupColliders,
		groupProjectiles,
		groupEnemies,
		groupBosses,
		groupBombs,
	};
	
	static TTF_Font* font;

private:

	int cnt = 0;
	SDL_Window* window;
};
enum class GameState {
	MENU,
	PLAYING,
	GAME_OVER
};

extern GameState currentState;
extern bool playerWon;