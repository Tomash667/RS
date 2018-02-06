#pragma once

struct Camera
{
	Vec3 from, to, up;

	Camera();
	Matrix GetViewMatrix();
};
