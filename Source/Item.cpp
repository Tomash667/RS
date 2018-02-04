#include "Pch.h"
#include "Item.h"

vector<Item> Item::items = {
	Item("gun", "Handgun", "gun.qmsh", "gun.png", IT_WEAPON),
	Item("ammo", "9mm ammo", "ammo.qmsh", "ammo.png", IT_AMMO),
	Item("food", "Canned food", "food.qmsh", "food.png", IT_FOOD)
};
