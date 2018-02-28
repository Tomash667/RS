#include "Pch.h"
#include "Level.h"
#include <Scene.h>
#include "GroundItem.h"


Level::Level(Scene* scene) : scene(scene)
{
	assert(scene);
}

Level::~Level()
{
	DeleteElements(items);
}

void Level::AddItem(GroundItem* item)
{
	assert(item);
	items.push_back(item);
	scene->Add(item->node);
}

void Level::RemoveItem(GroundItem* item)
{
	assert(item);
	scene->Remove(item->node);
	DeleteElement(items, item);
}
