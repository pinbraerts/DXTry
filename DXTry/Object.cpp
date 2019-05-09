#include "Object.hpp"
#include "Engine.hpp"

Object::Object(Engine& engine, const ObjectSerial& serial) {
	init(engine, serial);
}

void Object::init(Engine& engine, const ObjectSerial& serial) {
	Material::init(engine, serial.material);
	vertex_buffer = engine.create_buffer(D3D11_BIND_VERTEX_BUFFER, serial.vertices);
	index_buffer = engine.create_buffer(D3D11_BIND_INDEX_BUFFER, serial.indices);
	constant_buffers[0] = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(model));
	constant_buffers[1] = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(material));
	model = serial.model;
	n_indices = (UINT)serial.indices.size();
	stride = serial.stride;
	offset = serial.offset;
}

void Object::update(Engine& engine) {
	model = Matrix::CreateRotationY(XMConvertToRadians((float)engine.frames++)).Transpose();

	if (engine.frames == MAXUINT) engine.frames = 0;

	engine.context->UpdateSubresource(
		constant_buffers[0].Get(),
		0,
		nullptr,
		&model,
		0,
		0
	);
	engine.context->UpdateSubresource(
		constant_buffers[1].Get(),
		0,
		nullptr,
		&material,
		0,
		0
	);
}

void Material::init(Engine& engine, const MaterialSerial& serial) {
	auto [data, size] = engine.load_vertex(vertex_shader, serial.vertex_path);
	check engine.device->CreateInputLayout(
		serial.descriptors.data(),
		(UINT)serial.descriptors.size(),
		data.get(), size,
		&input_layout
	);

	engine.load_pixel(pixel_shader, serial.pixel_path);

	if (!serial.geometry_path.empty()) {
		engine.load_geometry(geometry_shader, serial.geometry_path);
		//stream_output = engine.create_buffer(
			//D3D11_BIND_STREAM_OUTPUT,
			//0xfffff);// (sizeof(Vector4) * 2 + sizeof(Vector3) * 2) * 36);
	}

	material = serial.constant;
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
	ID3D11Buffer* vscbs[] = { constant_buffers[0].Get() };
	engine.context->VSSetConstantBuffers(2, (UINT)std::size(vscbs), vscbs);
	ID3D11Buffer* pscbs[] = { constant_buffers[1].Get() };
	engine.context->PSSetConstantBuffers(3, (UINT)std::size(pscbs), pscbs);

	ID3D11Buffer* vertex_buffers[] {
		vertex_buffer.Get()
	};
	engine.context->IASetVertexBuffers(0, (UINT)std::size(vertex_buffers), vertex_buffers, &stride, &offset);

	engine.context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	engine.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	engine.context->IASetInputLayout(input_layout.Get());
	engine.context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	if (geometry_shader != nullptr) {
		//engine.context->GSSetConstantBuffers(2, (UINT)std::size(vscbs), vscbs);
		engine.context->GSSetShader(geometry_shader.Get(), nullptr, 0);
	}
	engine.context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	engine.context->DrawIndexed(n_indices, 0, 0);
}

Material::Material(Engine& engine, const MaterialSerial& serial) {
	init(engine, serial);
}
