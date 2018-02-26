#pragma once

struct Camera
{
	enum Mode
	{
		STATIC, // uses from, to, up
		THIRD_PERSON // uses target, target_h, up, rot, dist, shift
	};

	Camera();
	void Update(float dt);
	Matrix GetMatrix();
	void Reset() { reset = true; }

	Mode mode;
	SceneNode* target;
	Vec3 from, to, up;
	Vec2 rot;
	float springiness, dist, shift, target_h, fov, aspect, znear, zfar;

private:
	Vec3 vfrom, vto;
	Vec2 vrot;
	float vdist, vshift;
	bool reset;
};
