#pragma once

#include "Mesh.h"

struct MeshInstance;

struct SceneNode
{
	friend struct Scene;

	SceneNode(cstring debug_name = nullptr) : mesh(nullptr), inst(nullptr), rot(0.f), visible(true), attach_point(nullptr), tint(Color::White)
	{
#ifdef _DEBUG
		this->debug_name = debug_name;
#endif
	}
	~SceneNode();
	void AddChild(SceneNode* node, Mesh::Point* attach_point = nullptr);
	void SetMesh(Mesh* mesh);
	void SetMeshInstance(Mesh* mesh);
	Mesh* GetMesh() { return mesh; }
	MeshInstance* GetMeshInstance() { return inst; }
	vector<SceneNode*>& GetChildNodes() { return childs; }

#ifdef _DEBUG
	string debug_name;
#endif
	Vec3 pos;
	Vec3 tint;
	float rot;
	bool visible;
private:
	Mesh* mesh;
	MeshInstance* inst;
	vector<SceneNode*> childs;
	Mesh::Point* attach_point;
};
