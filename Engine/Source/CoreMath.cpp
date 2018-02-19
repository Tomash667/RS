#include "Pch.h"

const Int2 Int2::Zero = { 0,0 };
const Int2 Int2::MaxValue = { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };

const Rect Rect::Zero = { 0,0,0,0 };

const Vec2 Vec2::Zero = { 0.f, 0.f };
const Vec2 Vec2::One = { 1.f, 1.f };
const Vec2 Vec2::UnitX = { 1.f, 0.f };
const Vec2 Vec2::UnitY = { 0.f, 1.f };

const Vec3 Vec3::Zero = { 0.f, 0.f, 0.f };
const Vec3 Vec3::One = { 1.f, 1.f, 1.f };
const Vec3 Vec3::UnitX = { 1.f, 0.f, 0.f };
const Vec3 Vec3::UnitY = { 0.f, 1.f, 0.f };
const Vec3 Vec3::UnitZ = { 0.f, 0.f, 1.f };
const Vec3 Vec3::Up = { 0.f, 1.f, 0.f };
const Vec3 Vec3::Down = { 0.f, -1.f, 0.f };
const Vec3 Vec3::Right = { 1.f, 0.f, 0.f };
const Vec3 Vec3::Left = { -1.f, 0.f, 0.f };
const Vec3 Vec3::Forward = { 0.f, 0.f, -1.f };
const Vec3 Vec3::Backward = { 0.f, 0.f, 1.f };

const Vec4 Vec4::Zero = { 0.f, 0.f, 0.f, 0.f };
const Vec4 Vec4::One = { 1.f, 1.f, 1.f, 1.f };
const Vec4 Vec4::UnitX = { 1.f, 0.f, 0.f, 0.f };
const Vec4 Vec4::UnitY = { 0.f, 1.f, 0.f, 0.f };
const Vec4 Vec4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
const Vec4 Vec4::UnitW = { 0.f, 0.f, 0.f, 1.f };

const Matrix Matrix::IdentityMatrix = {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
};

const Quat Quat::Identity = { 0.f, 0.f, 0.f, 1.f };

RNG _RNG;

float ShortestArc(float a, float b)
{
	if(fabs(b - a) < PI)
		return b - a;
	if(b > a)
		return b - a - PI * 2.0f;
	return b - a + PI * 2.0f;
}
