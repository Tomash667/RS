#pragma once

struct Level
{
	Level(Scene* scene);
	~Level();
	void AddItem(GroundItem* item);
	void RemoveItem(GroundItem* item);

	Scene* scene;
	vector<GroundItem*> items;
};
