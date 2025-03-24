#pragma once
#pragma once

#include "ECS.h"
#include "Components.h"

class RevProjectile : public Component
{
public:
	RevProjectile(int s, int r) : speed(s), range(r)
	{
	}

	~RevProjectile()
	{
	}

	void init()override
	{
		transform = &entity->getComponent<TransformComponent>();
		transform->velocity.x -= speed;
	}

	void update() override
	{
		distance += speed;

		if (transform->position.x > Game::camera.x + Game::camera.w)
		{
			std::cout << "Out Of Bounds!" << std::endl;
			entity->destroy();
		}
		else if (distance > range)
		{
			std::cout << "Out Of Range" << std::endl;
			entity->destroy();
		}
	}

private:

	TransformComponent* transform;

	int speed = 0;
	int range = 0;
	int distance = 0;
};