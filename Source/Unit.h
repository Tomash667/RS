#pragma once

struct Unit
{
	cstring id, mesh_id;
	Mesh* mesh;

	Unit(cstring id, cstring mesh_id) : id(id), mesh_id(mesh_id), mesh(nullptr)
	{}

	static vector<Unit> units;
};
