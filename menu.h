#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <string>

class Menu {
public:
    SDL_Rect buttonRect;
    SDL_Texture* titleTexture;
    SDL_Texture* buttonTexture;
    SDL_Renderer* renderer;
    TTF_Font* font;

    Menu(SDL_Renderer* ren);
    ~Menu();

    void render();
    bool handleEvent(SDL_Event& e);
};