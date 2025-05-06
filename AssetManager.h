#pragma once
#pragma once

#include <map>
#include <string>
#include "TextureManager.h"
#include "Vector2D.h"
#include "ECS.h"
#include "SDL_ttf.h"

class AssetManager
{
public:

	AssetManager(Manager* manager);
	~AssetManager();
	SDL_Texture* GetTexture(std::string id);
	void CreateProjectile(Vector2D pos, int range, int speed, std::string texID);


	void CreateBomb(Vector2D pos, int range, int speed, std::string texID);

	void AddTexture(std::string id, const char* path);
	

	void AddFont(std::string id, std::string path, int fontSize);
	TTF_Font* GetFont(std::string id);
private:

	std::map<std::string, SDL_Texture*> textures;

	Manager* manager;
	std::map<std::string, TTF_Font*> fonts;
};