#include "Scene.hpp"
#include "Engine.hpp"

void Scene::create_cubes(Engine& engine) {
	MaterialData cube_material {
		L"light_pixel.cso",
		{
			{ 0.0215f, 0.1745f, 0.0215f, 1.0f }, // ambient
			{ 0.07568f, 0.61424f, 0.07568f, 1.0f }, // diffuse
			{ 0.633f, 0.727811f, 0.633f, 1.0f }, // specular
			{ 0.6f, 0.0f, 0.0f, 0.0f } // shininess
		}
	};
	MeshData cube_mesh {
		L"light_vertex.cso",
		L"light_geometry.cso",
		{ // descriptors
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, sizeof(Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		},
		{ // vertices
			-0.5f, -0.5f, -0.5f, 0, 0.75f, 0,
			-0.5f, -0.5f, 0.5f, 0, 1, 0,
			-0.5f, 0.5f, -0.5f, 0, 1, 0,
			-0.5f, 0.5f, 0.5f, 0, 1, 0,

			0.5f, -0.5f, -0.5f, 0, 1, 0,
			0.5f, -0.5f, 0.5f, 0, 1, 0,
			0.5f, 0.5f, -0.5f, 0, 1, 0,
			0.5f, 0.5f, 0.5f, 0, 1, 0,
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
		4 * 6
	};
	cubes.emplace_back(engine, ObjectData {
		std::make_shared<Mesh>(std::move(cube_mesh)),
		std::make_shared<Material>(std::move(cube_material)),
		Matrix()
	});
	for (size_t i = 0; i < 4; ++i) {
		cubes.push_back(cubes.back().clone(engine));
		cubes.back().model = Matrix::CreateTranslation(cubes.back().model.Transpose().Translation() + Vector3(2, 0, 0)).Transpose();
	}
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

	// children update
	for(auto& cube: cubes)
		cube.update(engine);
	lamp.update(engine);

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
	for (auto& cube : cubes)
		cube.render(engine);
	lamp.render(engine);
}

void Scene::create_lamp(Engine & engine) {
	MaterialData lamp_material {
		L"base_pixel.cso"
	};
	MeshData lamp_mesh {
		L"base_vertex.cso", L"",
		{ // descriptors
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				0, sizeof(Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		},
		{ // vertices
			-0.5f, -0.5f, -0.5f, 1, 1, 1,
			-0.5f, -0.5f, 0.5f, 1, 1, 1,
			-0.5f, 0.5f, -0.5f, 1, 1, 1,
			-0.5f, 0.5f, 0.5f, 1, 1, 1,

			0.5f, -0.5f, -0.5f, 1, 1, 1,
			0.5f, -0.5f, 0.5f, 1, 1, 1,
			0.5f, 0.5f, -0.5f, 1, 1, 1,
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
		4 * 6
	};
	lamp.set({
		std::make_unique<Mesh>(std::move(lamp_mesh)),
		std::make_unique<Material>(std::move(lamp_material)),
		Matrix()
	});
	lamp.init(engine);
}

void Scene::init(Engine& engine) {
	// init fields
	constant_buffers[0] = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(transform));
	constant_buffers[1] = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(light));
	camera.aspect_ratio = (float)engine.buffer_desc.Width / (float)engine.buffer_desc.Height;

	light = {
		Vector3(5, 5, 5),
		Vector3::Zero,
		Vector4(1, 1, 1, 1)
	};

	// init children
	create_cubes(engine);
	create_lamp(engine);
}
