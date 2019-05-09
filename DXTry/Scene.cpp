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
				0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		},
		{ // vertices
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, 0.5f,

			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
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
		sizeof(Vector3) * 1
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
	camera.FOV = engine.input.mouse.scroll / 100 +  70;

	// camera movement
	Vector2 mv {
		(float)engine.input.keyboard.pressed('W') - engine.input.keyboard.pressed('S'),
		(float)engine.input.keyboard.pressed('D') - engine.input.keyboard.pressed('A')
	};
	mv.Normalize();
	mv *= 100.0f * engine.delta_time; // speed
	camera.position += camera.direction() * mv.x + camera.left() * mv.y;

	// matrices update
	transform.view = camera.view();
	transform.projection = camera.projection();

	lights[0].position = camera.position + Vector4(0, 0, 0, lights[0].position.w);
	lights[0].direction = camera.direction() + Vector4(0, 0, 0, lights[0].direction.w);

	for (auto& light : lights) {
		light.eye = camera.position + Vector4(0, 0, 0, 1.0f);
		light.premultiply(transform.world);
	}

	// children update
	for(auto& cube: cubes)
		cube.update(engine);

	engine.context->UpdateSubresource(
		transform_buffer.Get(),
		0,
		nullptr,
		&transform,
		0,
		0
	);
	engine.context->UpdateSubresource(
		lights_buffer.Get(),
		0,
		nullptr,
		lights.data(),
		0,
		0
	);
}

void Scene::render(Engine& engine) {
	// set correct buffers
	ID3D11Buffer* pscbs[] {
		lights_buffer.Get()
	};
	engine.context->PSSetConstantBuffers(1, (UINT)std::size(pscbs), pscbs);

	ID3D11Buffer* vscbs[]{
		transform_buffer.Get()
	};
	engine.context->VSSetConstantBuffers(0, (UINT)std::size(vscbs), vscbs);

	// render children
	for (auto& cube : cubes)
		cube.render(engine);
}

void Scene::create_lights(Engine & engine) {
	lights.push_back({
		Vector4::Zero, // position, flashlight
		Vector4::Zero, // eye
		Vector4(1, 1, 1, 1), // ambient
		Vector4(0.8f, 0.8f, 0.8f, 1), // diffuse
		Vector4(1, 1, 1, 1), // specular
		Vector4(1, 0.09f, 0.032f, cosf(XMConvertToRadians(12.5f))), // attenuation, cut_off
		Vector4(0, 0, 0, cosf(XMConvertToRadians(17.5f))) // direction, outer
	});
	lights.push_back({
		Vector4(-1, -1, -1, 1), // direction
		Vector4::Zero, // eye
		Vector4(1, 1, 1, 1), // ambient
		Vector4(0.8f, 0.8f, 0.8f, 1), // diffuse
		Vector4(1, 1, 1, 1), // specular
	});
	lights.push_back({
		Vector4(3, 1, 3, 0), // position
		Vector4::Zero, // eye
		Vector4(1, 1, 1, 1), // ambient
		Vector4(0.8f, 0.8f, 0.8f, 1), // diffuse
		Vector4(1, 1, 1, 1), // specular
		Vector4(1, 0.09f, 0.032f, 0), // attenuation, cut_off
		Vector4(0, 0, 0, 0) // direction, outer
	});
}

void Scene::init(Engine& engine) {
	// init fields
	transform_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(transform));
	camera.aspect_ratio = (float)engine.buffer_desc.Width / (float)engine.buffer_desc.Height;

	// init children
	create_cubes(engine);
	create_lights(engine);

	lights_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, lights);
}
