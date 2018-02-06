#pragma once

struct Mesh;
struct MeshInstance;

struct SceneNode
{
	Mesh* mesh;
	MeshInstance* inst;
	Vec3 pos;
	float rot;

	SceneNode() : mesh(nullptr), inst(nullptr), rot(0.f) {}
	~SceneNode();
	void SetMeshInstance(Mesh* mesh);
};
