#pragma once
struct Animation {
	int speed;
	int frames;
	int index;
	Animation() {}
	Animation(int i, int f, int s) {
		index = i;
		frames = f;
		speed = s;
	}
};