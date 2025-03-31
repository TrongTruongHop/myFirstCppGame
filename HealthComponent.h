#pragma once
#include "ECS.h"
#include <iostream>
#include "Game.h"
class HealthComponent : public Component {
public:
    int health;
    int maxHealth;

    HealthComponent(int hp) {
        maxHealth = hp;
        health = hp;
    }

    void takeDamage(int amount,bool endGame) {
        health -= amount;
        std::cout << "Entity took " << amount << " damage! HP: " << health << "/" << maxHealth << std::endl;

        if (health <= 0) {
            std::cout << "Entity destroyed!\n";
            entity->destroy();
			if (endGame) Game::isRunning = false;
        }
    }
};