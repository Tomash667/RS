#pragma once

struct Item
{
	enum class Type
	{
		WEAPON,
		AMMO,
		FOOD
	};

	cstring id, name, mesh_id, tex_id;
	Mesh* mesh;
	Texture* tex;
	Type type;
	uint count, max_stack;

	Item(cstring id, cstring name, cstring mesh_id, cstring tex_id, uint count, uint max_stack, Type type) : id(id), name(name), mesh_id(mesh_id),
		tex_id(tex_id), count(count), max_stack(max_stack), type(type), mesh(nullptr), tex(nullptr)
	{
	}
	bool IsStackable() const { return type != Type::WEAPON; }

	static vector<Item> items;
	static Item* Get(Cstring id);
};
