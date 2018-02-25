#include "Pch.h"
#include "Item.h"

vector<Item> Item::items = {
	Item("gun", "Handgun", "m1911.qmsh", "colors.png"/*"gun.qmsh", "gun.png"*/, 10, IT_WEAPON),
	Item("ammo", "Handgun ammo", "m1911.qmsh", "colors.png"/*"ammo.qmsh", "ammo.png"*/, 40, IT_AMMO),
	Item("food", "Canned food", "m1911.qmsh", "colors.png"/*"food.qmsh", "food.png"*/, 5, IT_FOOD)
};
