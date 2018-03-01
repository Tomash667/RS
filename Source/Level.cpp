#include "Pch.h"
#include "Level.h"
#include <Scene.h>
#include <SceneNode.h>
#include "GroundItem.h"
#include "Item.h"


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

void Level::Save(FileWriter& f)
{
	f << items.size();
	for(GroundItem* item : items)
	{
		f << item->item->id;
		f << item->count;
		f << item->ammo_count;
		f << item->node->pos;
		f << item->node->rot;
	}
}

void Level::Load(FileReader& f)
{
	uint count;
	f >> count;
	items.reserve(count);
	for(uint i = 0; i < count; ++i)
	{
		Ptr<GroundItem> item;
		item->item = Item::Get(f.ReadString1());
		f >> item->count;
		f >> item->ammo_count;
		item->node = new SceneNode;
		item->node->SetMesh(item->item->mesh);
		f >> item->node->pos;
		f >> item->node->rot;
		scene->Add(item->node);
		items.push_back(item.Pin());
	}
}
