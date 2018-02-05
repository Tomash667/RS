#pragma once

#include "Mesh.h"

struct MeshInstance
{
	MeshInstance(Mesh* mesh) : mesh(mesh) {}

	Mesh* mesh;
};
