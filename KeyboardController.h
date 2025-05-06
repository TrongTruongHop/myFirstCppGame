#pragma once

#include "Game.h"
#include "ECS.h"
#include "Components.h"
#include <chrono>

class KeyboardController : public Component {
public:
	TransformComponent* transform;
	SpriteComponent* sprite;
	int bulletCooldown = 300;
	int bombCooldown = 10000;
	std::chrono::steady_clock::time_point lastBulletTime;
	std::chrono::steady_clock::time_point lastBombTime;

	void init() override {
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
		lastBulletTime = std::chrono::steady_clock::now();
		lastBombTime = std::chrono::steady_clock::now();
	}
	void update() override {
		if (Game::event.type == SDL_KEYDOWN) {
			switch (Game::event.key.keysym.sym) {
			case SDLK_w:
				transform->velocity.y = -1;
				sprite->Play("Walk");
				break;
			case SDLK_a:
				transform->velocity.x = -1;
				sprite->Play("Walk");
				sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
				break;
			case SDLK_d:
				transform->velocity.x = 1;
				sprite->Play("Walk");
				break;
			case SDLK_s:
				transform->velocity.y = 1;
				sprite->Play("Walk");
				break;
			default:
				break;
			}
		}
		if (Game::event.type == SDL_MOUSEBUTTONDOWN) {
			if (Game::event.button.button == SDL_BUTTON_LEFT) {
				auto now = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBulletTime).count();
				if (elapsed < bulletCooldown) return;
				lastBulletTime = std::chrono::steady_clock::now();
				Game::assets->CreateProjectile(Vector2D(transform->position.x + 40, transform->position.y + 30), 1000, 2, "projectile");
			}
			if (Game::event.button.button == SDL_BUTTON_RIGHT) {
			
				auto now = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBombTime).count();
				if (elapsed < bombCooldown) return;
				lastBombTime = std::chrono::steady_clock::now();
				Game::assets->CreateBomb(Vector2D(transform->position.x + 40, transform->position.y + 30), 1000, 2, "bomb");
			}
			
		}
		if (Game::event.type == SDL_KEYUP) {
			switch (Game::event.key.keysym.sym) {
			case SDLK_w:
				transform->velocity.y = 0;
				sprite->Play("Idle");
				break;
			case SDLK_a:
				transform->velocity.x = 0;
				sprite->Play("Idle");
				sprite->spriteFlip = SDL_FLIP_NONE;
				break;
			case SDLK_d:
				transform->velocity.x = 0;
				sprite->Play("Idle");
				break;
			case SDLK_s:
				transform->velocity.y = 0;
				sprite->Play("Idle");
				break;
			case SDLK_ESCAPE:
				Game::isRunning = false;
				break;
			default:
				break;
			}
		}
	}
};