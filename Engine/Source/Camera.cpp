#include "Pch.h"
#include "Camera.h"
#include "SceneNode.h"

Camera::Camera() : mode(STATIC), from(0, 2, -2), vfrom(from), to(0, 0, 0), vto(to), up(0, 1, 0), rot(0, 4.2875104f), vrot(rot), springiness(40.f), dist(2.f),
target_h(1.5f), shift(0.f), reset(true), target(nullptr)
{
}

void Camera::Update(float dt)
{
	float t = 1.0f - exp(log(0.5f) * springiness * dt);
	if(mode == STATIC)
	{
		if(reset)
		{
			vto = to;
			vfrom = from;
			reset = false;
		}
		else
		{
			vfrom += (from - vfrom) * t;
			vto += (to - vto) * t;
		}
	}
	else
	{
		assert(target);
		if(reset)
		{
			vrot = rot;
			vdist = dist;
			vshift = shift;
		}
		else
		{
			vrot = Vec2::Slerp(vrot, rot, t);
			vdist += (dist - vdist) * t;
			vshift += (shift - vshift) * t;
		}

		to = target->pos;
		to.y += target_h;
		if(shift != 0.f)
		{
			float angle = vrot.x - PI / 2;
			to += Vec3(sin(angle)*vshift, 0, cos(angle)*vshift);
		}

		Vec3 ray(0, -vdist, 0);
		Matrix mat = Matrix::Rotation(vrot.x, vrot.y, 0);
		ray = Vec3::Transform(ray, mat);
		from = to + ray;

		if(reset)
		{
			vto = to;
			vfrom = from;
			reset = false;
		}
		else
		{
			vfrom += (from - vfrom) * t;
			vto += (to - vto) * t;
		}
	}
}

Matrix Camera::GetViewMatrix()
{
	return Matrix::CreateLookAt(vfrom, vto, up);
}
