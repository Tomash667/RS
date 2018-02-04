#pragma once

struct Object
{
	cstring id, mesh_id;
	Mesh* mesh;

	Object(cstring id, cstring mesh_id) : id(id), mesh_id(mesh_id), mesh(nullptr)
	{}

	static vector<Object> objects;
};
