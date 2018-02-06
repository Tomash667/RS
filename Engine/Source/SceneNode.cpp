#include "Pch.h"
#include "SceneNode.h"
#include "MeshInstance.h"

SceneNode::~SceneNode()
{
	delete inst;
}

void SceneNode::SetMeshInstance(Mesh* mesh)
{
	assert(mesh);
	this->mesh = mesh;
	this->inst = new MeshInstance(mesh);
}
