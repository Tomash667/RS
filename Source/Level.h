#pragma once

struct Level
{
	Level(Scene* scene);
	~Level();
	void AddItem(GroundItem* item);
	void RemoveItem(GroundItem* item);
	void Save(FileWriter& f);
	void Load(FileReader& f);

	Scene* scene;
	vector<GroundItem*> items;
};
