#include "Pch.h"
#include "Item.h"

vector<Item> Item::items = {
	Item("gun", "Handgun", "m1911.qmsh", "colors.png"/*"gun.qmsh", "gun.png"*/, 10, 1, Type::WEAPON),
	Item("ammo", "Handgun ammo", "m1911.qmsh", "colors.png"/*"ammo.qmsh", "ammo.png"*/, 10, 40, Type::AMMO),
	Item("food", "Canned food", "m1911.qmsh", "colors.png"/*"food.qmsh", "food.png"*/, 1, 5, Type::FOOD)
};

Item* Item::Get(Cstring id)
{
	for(Item& item : items)
	{
		if(item.id == id)
			return &item;
	}

	throw Format("Missing item '%s'.", id);
}
