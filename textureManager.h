#pragma once

#include"Game.h";

class textureManager {

public:
	static SDL_Texture* LoadTexture(const char* filename, SDL_Renderer* ren);

};