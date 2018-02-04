#pragma once

typedef const char* cstring;
typedef unsigned int uint;

struct Vec3
{
	float x, y, z;

	Vec3() {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};
