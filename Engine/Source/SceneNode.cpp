#include "Pch.h"
#include "SceneNode.h"
#include "MeshInstance.h"

SceneNode::~SceneNode()
{
	delete inst;
	DeleteElements(childs);
}

void SceneNode::AddChild(SceneNode* node, Mesh::Point* attach_point)
{
	assert(node);
	childs.push_back(node);
	assert(mesh && mesh->HavePoint(attach_point));
	node->attach_point = attach_point;
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
