#pragma once

struct Resource
{
	enum Type
	{
		Texture,
		Mesh
	};

	virtual ~Resource() {}

	string name;
	Type type;
};
