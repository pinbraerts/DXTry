#include "Light.hpp"
#include "Engine.hpp"

void Light::premultiply(const Matrix& world) {
	Matrix tr = world.Transpose();

	float w = position.w;
	position.w = 1;
	position = Vector4::Transform(position, tr);
	position.w = w;

	w = eye.w;
	eye.w = 1;
	eye = Vector4::Transform(eye, tr);
	eye.w = w;

	w = direction.w;
	direction.w = 1;
	direction = Vector4::Transform(direction, tr);
	direction.w = w;
}
