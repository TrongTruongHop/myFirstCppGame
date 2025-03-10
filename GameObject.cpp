#include"GameObject.h";
#include"textureManager.h"

GameObject::GameObject(const char* texturesheet,int x, int y ) {
	
	objTexture = textureManager::LoadTexture(texturesheet);

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
	SDL_RenderCopy(Game::renderer, objTexture, &srcRect, &destRect);
}