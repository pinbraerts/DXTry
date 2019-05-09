#include "Scene.hpp"
#include "Engine.hpp"

void Scene::create_cube(Engine& engine) {
	ObjectSerial serial {
		{
			L"light_vertex.cso",
			L"light_pixel.cso",
			{ // descriptors
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
					0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
					0, sizeof(Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
			},
			{
				0.1f, // ambient strength
				1.0f, // diffuse strength
				0.5f, // specular strength
				256 // shininess
			},
			L"light_geometry.cso"
		},
		{ // vertices
			-0.5f, -0.5f, -0.5f, 0, 0, 0,
			-0.5f, -0.5f, 0.5f, 0, 0, 1,
			-0.5f, 0.5f, -0.5f, 0, 1, 0,
			-0.5f, 0.5f, 0.5f, 0, 1, 1,

			0.5f, -0.5f, -0.5f, 1, 0, 0,
			0.5f, -0.5f, 0.5f, 1, 0, 1,
			0.5f, 0.5f, -0.5f, 1, 1, 0,
			0.5f, 0.5f, 0.5f, 1, 1, 1,
		},
		{ // indices
			0, 2, 1, // -x
			1, 2, 3,

			4, 5, 6, // +x
			5, 7, 6,

			0, 1, 5, // -y
			0, 5, 4,

			2, 6, 7, // +y
			2, 7, 3,

			0, 4, 6, // -z
			0, 6, 2,

			1, 3, 7, // +z
			1, 7, 5
		},
		Matrix(),
		4 * 6
	};
	cube.init(engine, serial);
}

void Scene::update(Engine& engine) {
	// camera rotation
	camera.yaw = -engine.input.mouse.position.x * engine.sensivity;
	camera.pitch = engine.input.mouse.position.y * engine.sensivity;

	// camera movement
	Vector2 mv {
		(float)engine.input.keyboard.pressed('W') - engine.input.keyboard.pressed('S'),
		(float)engine.input.keyboard.pressed('D') - engine.input.keyboard.pressed('A')
	};
	mv.Normalize();
	mv *= 1000.0f * engine.delta_time; // speed
	camera.position += camera.direction() * mv.x + camera.left() * mv.y;

	// matrices update
	transform.view = camera.view();
	transform.projection = camera.projection();

	light.eye = camera.position;
	light.light_position = Vector3::Transform(Vector3(1, 1, 0), transform.world * transform.view);

	// children update
	cube.update(engine);

	engine.context->UpdateSubresource(
		constant_buffers[0].Get(),
		0,
		nullptr,
		&transform,
		0,
		0
	);
	engine.context->UpdateSubresource(
		constant_buffers[1].Get(),
		0,
		nullptr,
		&light,
		0,
		0
	);
}

void Scene::render(Engine& engine) {
	// set correct buffers
	engine.context->VSSetConstantBuffers(0, (UINT)std::size(constant_buffers), (ID3D11Buffer**)constant_buffers);
	engine.context->GSSetConstantBuffers(0, (UINT)std::size(constant_buffers), (ID3D11Buffer**)constant_buffers);

	ID3D11Buffer* pscbs[] {
		constant_buffers[1].Get()
	};
	engine.context->PSSetConstantBuffers(1, (UINT)std::size(pscbs), pscbs);

	// render children
	cube.render(engine);
}

void Scene::init(Engine& engine) {
	// init fields
	constant_buffers[0] = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(transform));
	constant_buffers[1] = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(light));
	camera.aspect_ratio = (float)engine.buffer_desc.Width / (float)engine.buffer_desc.Height;

	// init children
	create_cube(engine);

	light = {
		Vector3::Zero,
		Vector3::Zero,
		Vector4(1, 1, 1, 1)
	};
}
