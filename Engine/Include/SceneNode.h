#pragma once

struct Mesh;
struct MeshInstance;

struct SceneNode
{
	Mesh* mesh;
	MeshInstance* inst;
	Vec3 pos;
	float rot;

	void SetMeshInstance(Mesh* mesh);
};
