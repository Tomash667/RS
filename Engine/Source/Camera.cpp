#include "Pch.h"
#include "Camera.h"

Camera::Camera() : from(0, 2, -2), to(0, 0, 0), up(0, 1, 0), rot(0, 4.2875104f)
{}

Matrix Camera::GetViewMatrix()
{
	return Matrix::CreateLookAt(from, to, up);
}
