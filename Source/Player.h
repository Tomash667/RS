#pragma once

struct Player
{
	enum Animation
	{
		A_NONE,
		A_STAND,
		A_WALK,
		A_WALK_BACK,
		A_WALK_LEFT,
		A_WALK_RIGHT,
		A_ROTATE_LEFT,
		A_ROTATE_RIGHT,
		A_RUN,
		A_RUN_LEFT,
		A_RUN_RIGHT,
		A_IDLE,
		A_STAND_GUN,
		A_AIM,
		A_SHOOT,
		A_RELOAD,
		A_HURT,
		A_DIE,
		A_PUNCH
	};

	SceneNode* node;
	float hp, hpmax, food, foodmax;
	Animation anim, new_anim;
	GroundItem* item_before;

	Player() : hp(60.f), hpmax(100.f), food(33.f), foodmax(100.f), anim(A_NONE), new_anim(A_STAND), item_before(nullptr) {}
};
