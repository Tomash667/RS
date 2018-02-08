#pragma once

struct Camera
{
	Vec3 from, to, up;
	Vec2 rot;

	Camera();
	Matrix GetViewMatrix();
};
