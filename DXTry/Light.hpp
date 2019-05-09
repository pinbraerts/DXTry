#ifndef DXTRY_POINT_LIGHT
#define DXTRY_POINT_LIGHT

#include "includes.hpp"
#include "Object.hpp"

struct Light {
	Vector4 position;
	Vector4 eye;
	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	Vector4 attenuation;
	Vector4 direction;

	void premultiply(const Matrix& world);
};

#endif // !DXTRY_POINT_LIGHT
