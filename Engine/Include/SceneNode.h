#pragma once

struct Mesh;
struct MeshInstance;

struct SceneNode
{
	SceneNode(cstring debug_name = nullptr) : mesh(nullptr), inst(nullptr), rot(0.f), visible(true)
	{
#ifdef _DEBUG
		this->debug_name = debug_name;
#endif
	}
	~SceneNode();
	void AddChild(SceneNode* node);
	void SetMesh(Mesh* mesh);
	void SetMeshInstance(Mesh* mesh);
	Mesh* GetMesh() { return mesh; }
	MeshInstance* GetMeshInstance() { return inst; }
	vector<SceneNode*>& GetChildNodes() { return childs; }

#ifdef _DEBUG
	string debug_name;
#endif
	Vec3 pos;
	float rot;
	bool visible;
private:
	Mesh* mesh;
	MeshInstance* inst;
	vector<SceneNode*> childs;
};
