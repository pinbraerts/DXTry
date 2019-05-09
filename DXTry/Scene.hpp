#ifndef DXTRY_SCENE_HPP
#define DXTRY_SCENE_HPP

#include "includes.hpp"
#include "Camera.hpp"
#include "Object.hpp"

struct Scene {
	struct {
		Matrix world;
		Matrix view;
		Matrix projection;
	} transform;

	struct {
		Vector3 light_position;
		Vector3 eye;
		Vector4 light_color;
		Vector2 _;
	} light;

	ComPtr<ID3D11Buffer> constant_buffers[2];
	Camera camera;

	Object cube;
	void create_cube(Engine& engine);

	void init(Engine& engine);
	void update(Engine& engine);
	void render(Engine& engine);
	// void destroy(Engine& engine);
};

#endif // !DXTRY_SCENE_HPP
