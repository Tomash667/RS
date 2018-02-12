#include "Pch.h"
#include "SceneNode.h"
#include "MeshInstance.h"

SceneNode::~SceneNode()
{
	delete inst;
	DeleteElements(childs);
}

void SceneNode::AddChild(SceneNode* node)
{
	assert(node);
	childs.push_back(node);
}

void SceneNode::SetMesh(Mesh* mesh)
{
	assert(mesh && !mesh->IsAnimated());
	this->mesh = mesh;
	this->inst = nullptr;
}

void SceneNode::SetMeshInstance(Mesh* mesh)
{
	assert(mesh && mesh->IsAnimated());
	this->mesh = mesh;
	this->inst = new MeshInstance(mesh);
}
