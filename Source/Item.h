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

	Item(cstring id, cstring name, cstring mesh_id, cstring tex_id, ITEM_TYPE type) : id(id), name(name), mesh_id(mesh_id), tex_id(tex_id), type(type), mesh(nullptr),
		tex(nullptr)
	{}

	static vector<Item> items;
};
