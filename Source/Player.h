#pragma once

struct Player
{
	SceneNode* node;
	float hp, hpmax, food, foodmax;
	bool moving;

	Player() : hp(60.f), hpmax(100.f), food(33.f), foodmax(100.f), moving(false) {}
};
