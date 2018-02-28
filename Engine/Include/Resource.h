#pragma once

struct Resource
{
	enum Type
	{
		Texture,
		Mesh,
		Sound
	};

	virtual ~Resource() {}

	string name;
	Type type;
};
