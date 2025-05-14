#include "Menu.h"
#include "textureManager.h"

Menu::Menu(SDL_Renderer* ren) {
    renderer = ren;
    font = TTF_OpenFont("assets/OpenSans.ttf", 48);

    SDL_Color white = { 255, 255, 255 };

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Pixel Blade", white);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_FreeSurface(titleSurface);

    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Start Game", white);
    buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_FreeSurface(buttonSurface);

    buttonRect = { 300, 400, 200, 80 };
}

Menu::~Menu() {
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(buttonTexture);
    TTF_CloseFont(font);
}

void Menu::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    SDL_Rect titleRect = { 250, 150, 300, 100 };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);
}

bool Menu::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x;
        int y = e.button.y;
        if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
            y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
            return true; // Start game
        }
    }
    return false;
}