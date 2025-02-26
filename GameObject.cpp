#include"GameObject.h";
#include"textureManager.h"

GameObject::GameObject(const char* texturesheet, SDL_Renderer* ren,int x, int y ) {
	renderer = ren;
	objTexture = textureManager::LoadTexture(texturesheet, ren);

	xpos = x;
	ypos = y;
}
void GameObject::Update(){

	xpos++;
	ypos++;
	srcRect.h = 1152;
	srcRect.w = 648;
	srcRect.x = 0;
	srcRect.y = 0;

	destRect.x = xpos;
	destRect.y = ypos;
	destRect.w = 64;
	destRect.h = 64; 
}
void GameObject::Render() {
	SDL_RenderCopy(renderer, objTexture, &srcRect, &destRect);
}