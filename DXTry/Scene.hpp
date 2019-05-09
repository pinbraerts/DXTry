#ifndef DXTRY_SCENE_HPP
#define DXTRY_SCENE_HPP

#include "includes.hpp"
#include "Camera.hpp"
#include "Object.hpp"
#include "Light.hpp"

struct Scene: IObject {
	struct {
		Matrix world;
		Matrix view;
		Matrix projection;
	} transform;

	ComPtr<ID3D11Buffer> transform_buffer;
	ComPtr<ID3D11Buffer> lights_buffer;
	Camera camera;

	std::vector<Object> cubes;
	void create_cubes(Engine& engine);

	std::vector<Light> lights;
	void create_lights(Engine& engine);

	void init(Engine& engine) override;
	void update(Engine& engine) override;
	void render(Engine& engine) override;
	// void destroy(Engine& engine) override;
};

#endif // !DXTRY_SCENE_HPP
