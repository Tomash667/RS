#include "Pch.h"
#include "SceneNode.h"
#include "MeshInstance.h"

void SceneNode::SetMeshInstance(Mesh* mesh)
{
	assert(mesh);
	this->mesh = mesh;
	this->inst = new MeshInstance(mesh);
}
