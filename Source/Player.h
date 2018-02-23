#pragma once

struct Player
{
	enum class Animation
	{
		NONE,
		STAND,
		WALK,
		WALK_BACK,
		WALK_LEFT,
		WALK_RIGHT,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		RUN,
		RUN_LEFT,
		RUN_RIGHT,
		IDLE,
		STAND_GUN,
		AIM,
		SHOOT,
		RELOAD,
		HURT,
		DIE,
		PUNCH
	};

	enum class Action
	{
		NONE,
		PICKUP
	};

	SceneNode* node;
	float hp, hpmax, food, foodmax;
	Action action;
	Animation anim, new_anim;
	GroundItem* item_before;

	Player() : hp(60.f), hpmax(100.f), food(33.f), foodmax(100.f), action(Action::NONE), anim(Animation::NONE), new_anim(Animation::STAND),
		item_before(nullptr) {}
};
