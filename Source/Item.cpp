#include "Pch.h"
#include "Item.h"

vector<Item> Item::items = {
	Item("gun", "Handgun", "m1911.qmsh", "colors.png"/*"gun.qmsh", "gun.png"*/, 10, 1, IT_WEAPON),
	Item("ammo", "Handgun ammo", "m1911.qmsh", "colors.png"/*"ammo.qmsh", "ammo.png"*/, 10, 40, IT_AMMO),
	Item("food", "Canned food", "m1911.qmsh", "colors.png"/*"food.qmsh", "food.png"*/, 1, 5, IT_FOOD)
};

Item* Item::Get(Cstring id)
{
	for(Item& item : items)
	{
		if(item.id == id)
			return &item;
	}

	assert(0);
	return &items[0];
}
