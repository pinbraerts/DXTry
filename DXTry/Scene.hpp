#ifndef DXTRY_SCENE_HPP
#define DXTRY_SCENE_HPP

#include "includes.hpp"
#include "Camera.hpp"
#include "Object.hpp"

struct Scene {
	struct ConstantBufferStruct {
		Matrix world;
		Matrix view;
		Matrix projection;
	} matrices;

	ComPtr<ID3D11Buffer> constant_buffer;
	Camera camera;

	Object cube;
	void create_cube(Engine& engine);

	void create_constant_buffer(Engine& engine);

	void init(Engine& engine);
	void update(Engine& engine);
	void render(Engine& engine);
	// void destroy(Engine& engine);
};

#endif // !DXTRY_SCENE_HPP
