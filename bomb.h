#pragma once

#include "ECS.h"
#include "Components.h"

class bomb : public Component
{
public:
	bomb(int s, int r) : speed(s), range(r)
	{
	}

	~bomb()
	{
	}

	void init()override
	{
		transform = &entity->getComponent<TransformComponent>();
		transform->velocity.x += (speed/2);
	}

	void update() override
	{
		distance += speed;


	}

private:

	TransformComponent* transform;

	int speed = 0;
	int range = 0;
	int distance = 0;
};