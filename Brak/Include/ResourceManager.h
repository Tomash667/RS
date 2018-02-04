#pragma once

struct Texture;
struct Mesh;

struct ResourceManager
{
	Texture* GetTexture(cstring name);
	Mesh* GetMesh(cstring name);
};
