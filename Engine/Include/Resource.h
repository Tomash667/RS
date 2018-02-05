#pragma once

struct Resource
{
	string name;

	virtual bool Load() = 0;
};
