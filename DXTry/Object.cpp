#include "Object.hpp"
#include "Engine.hpp"

void Object::set(ObjectData&& data) {
	ObjectData::operator=(std::move(data));
}

Object::Object(ObjectData&& data): ObjectData(std::move(data)) { }

Object::Object(Engine& engine, ObjectData&& data) : ObjectData(std::move(data)) {
	init(engine);
}

void Object::init(Engine& engine) {
	material->init(engine);
	mesh->init(engine);
	constant_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(model));
}

void Object::update(Engine& engine) {
	//model = Matrix::CreateRotationY(XMConvertToRadians((float)engine.frames++)).Transpose();

	if (engine.frames == MAXUINT) engine.frames = 0;

	engine.context->UpdateSubresource(
		constant_buffer.Get(),
		0,
		nullptr,
		&model,
		0,
		0
	);
}

Object Object::clone(Engine& engine) {
	Object res;
	res.material = material;
	res.mesh = mesh;
	res.constant_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(model));
	res.model = model;
	return res;
}

void Object::render(Engine& engine) {
	/*D3D11_RASTERIZER_DESC desc {
		D3D11_FILL_SOLID,
		D3D11_CULL_NONE,
		false,
		0,
		0.0f,
		0.0f,
		true,
		false,
		false,
		false
	};
	ID3D11RasterizerState* rs;
	HRESULT hr = engine.device->CreateRasterizerState(
		&desc,
		&rs
	);
	if (FAILED(hr))
		throw Engine::Error(hr);
	engine.context->RSSetState(rs);
	rs->Release();*/

	// set correct buffers
	ID3D11Buffer* vscbs[] {
		constant_buffer.Get()
	};
	engine.context->VSSetConstantBuffers(2, (UINT)std::size(vscbs), vscbs);

	material->render(engine);
	mesh->render(engine);
}
