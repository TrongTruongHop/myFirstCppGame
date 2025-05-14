#include "Game.h"
#include "Menu.h"
Game* game = nullptr;

int main(int argc, char* argv[])
{
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	game = new Game();
	game->init("FURY", 800, 640, false);
	Menu* mainMenu = new Menu(Game::renderer);
	while (game->running())
	{
		SDL_Event event;
		frameStart = SDL_GetTicks();
		game->handleEvents();
		game->update();
		game->render();
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
		SDL_RenderPresent(Game::renderer);
	}

	game->clean();
	return 0;
}