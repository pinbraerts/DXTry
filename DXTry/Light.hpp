#ifndef DXTRY_POINT_LIGHT
#define DXTRY_POINT_LIGHT

#include "includes.hpp"
#include "Object.hpp"

struct LightData {
	Vector4 position;
	Vector4 eye;

	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	Vector4 _align;
};

struct Light: LightData, Object {
	ComPtr<ID3D11Buffer> constant_light;

	void set(ObjectData&& data, LightData l);

	void init(Engine& engine) override;
	void update(Engine& engine) override;
	void render(Engine& engine) override;
};

#endif // !DXTRY_POINT_LIGHT
