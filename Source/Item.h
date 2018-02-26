#pragma once

enum ITEM_TYPE
{
	IT_WEAPON,
	IT_AMMO,
	IT_FOOD
};

struct Item
{
	cstring id, name, mesh_id, tex_id;
	Mesh* mesh;
	Texture* tex;
	ITEM_TYPE type;
	uint count, max_stack;

	Item(cstring id, cstring name, cstring mesh_id, cstring tex_id, uint count, uint max_stack, ITEM_TYPE type) : id(id), name(name), mesh_id(mesh_id),
		tex_id(tex_id), count(count), max_stack(max_stack), type(type), mesh(nullptr), tex(nullptr)
	{
	}

	static vector<Item> items;
	static Item* Get(Cstring id);
};
