#pragma once

struct UnitItem
{
	Item* item;
	uint count;
};

struct UnitEntity
{
	SceneNode* node;
	Unit* data;
	UnitItem weapon;
	vector<UnitItem> items;
	float hp, hpmax;
};
