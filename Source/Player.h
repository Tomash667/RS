#pragma once

struct ItemSlot
{
	Item* item;
	uint count;
	uint ammo_count;

	ItemSlot(Item* item, uint count, uint ammo_count) : item(item), count(count), ammo_count(ammo_count) {}
};

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
		PUNCH,
		ACTION
	};

	enum class Action
	{
		NONE,
		PICKUP
	};

	SceneNode* node;
	float hp, hpmax, food, foodmax;
	Action action;
	int action_state, weapon_index;
	Animation anim, new_anim;
	GroundItem* item_before;
	vector<ItemSlot> items;

	Player();
	float GetHpP() const { return hp / hpmax; }
	float GetFoodP() const { return food / foodmax; }
	bool HaveWeapon() const { return weapon_index != -1; }
	ItemSlot& GetWeapon() { assert(HaveWeapon()); return items[weapon_index]; }
	uint GetAmmoLeft(Item* item) const;
	uint AddItem(Item* item, uint count, uint ammo_count);
};
