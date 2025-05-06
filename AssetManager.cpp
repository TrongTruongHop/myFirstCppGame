#include "AssetManager.h"
#include "Components.h"

AssetManager::AssetManager(Manager* man) : manager(man)
{
}

AssetManager::~AssetManager()
{
}

void AssetManager::CreateProjectile(Vector2D pos, int range, int speed, std::string texID)
{
	auto& proj(manager->addEntity());
	proj.addComponent<TransformComponent>(pos.x, pos.y, 32, 32, 1);
	proj.addComponent<SpriteComponent>(texID, false);
	proj.addComponent<Projectile>(speed, range);
	proj.addComponent<ColliderComponent>("projectile");
	proj.addGroup(Game::groupProjectiles);

}
void AssetManager::CreateBomb(Vector2D pos, int range, int speed, std::string texID)
{
	auto& bom(manager->addEntity());
	bom.addComponent<TransformComponent>(pos.x, pos.y, 32, 32, 1);
	bom.addComponent<SpriteComponent>(texID, false);
	bom.addComponent<bomb>(speed, range);
	bom.addComponent<ColliderComponent>("bomb");
	bom.addGroup(Game::groupBombs);

}
void AssetManager::AddTexture(std::string id, const char* path)
{
	textures.emplace(id, textureManager::LoadTexture(path));
}
void AssetManager::AddFont(std::string id, std::string path, int fontSize)
{
	fonts.emplace(id, TTF_OpenFont(path.c_str(), fontSize));
}
TTF_Font* AssetManager::GetFont(std::string id)
{
	return fonts[id];
}
SDL_Texture* AssetManager::GetTexture(std::string id)
{
	return textures[id];
}