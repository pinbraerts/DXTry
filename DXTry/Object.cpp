#include "Object.hpp"
#include "Engine.hpp"

Layout::Layout(
	ComPtr<ID3D11VertexShader> _vertex_shader,
	ComPtr<ID3D11PixelShader> _pixel_shader,
	ComPtr<ID3D11InputLayout> _input_layout
) : vertex_shader(_vertex_shader),
	pixel_shader(_pixel_shader),
	input_layout(_input_layout) {}

Object::Object(Engine& engine, const ObjectSerial& serial) :
	Layout(create_layout(engine, serial)),
	vertex_buffer(engine.create_buffer(D3D11_BIND_VERTEX_BUFFER, serial.vertices)),
	index_buffer(engine.create_buffer(D3D11_BIND_INDEX_BUFFER, serial.indices)),
	model(serial.model),
	n_indices((UINT)serial.indices.size()),
	stride(serial.stride),
	offset(serial.offset) {}

void Object::init(Engine& engine, const ObjectSerial& serial) {
	Layout::operator=(create_layout(engine, serial));
	vertex_buffer = engine.create_buffer(D3D11_BIND_VERTEX_BUFFER, serial.vertices);
	index_buffer = engine.create_buffer(D3D11_BIND_INDEX_BUFFER, serial.indices);
	constant_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(Matrix));
	model = serial.model;
	n_indices = (UINT)serial.indices.size();
	stride = serial.stride;
	offset = serial.offset;
}

void Object::update(Engine& engine) {
	model = Matrix::CreateRotationY(XMConvertToRadians((float)engine.frames++)).Transpose();

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

Layout Object::create_layout(Engine& engine, const ObjectSerial& serial) {
	Layout res;
	std::string_view shb = Engine::load_vertex(
		engine.device.Get(),
		res.vertex_shader,
		serial.vertex_path
	);

	check engine.device->CreateInputLayout(
		serial.descriptors.data(),
		(UINT)serial.descriptors.size(),
		shb.data(),
		shb.size(),
		&res.input_layout
	);

	delete[] shb.data();

	delete[] Engine::load_pixel(
		engine.device.Get(),
		res.pixel_shader,
		serial.pixel_path
	).data();
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
	ID3D11Buffer* constant_buffers[] {
		constant_buffer.Get()
	};
	engine.context->VSSetConstantBuffers(1, (UINT)std::size(constant_buffers), constant_buffers);

	ID3D11Buffer* vertex_buffers[] {
		vertex_buffer.Get()
	};
	engine.context->IASetVertexBuffers(0, (UINT)std::size(vertex_buffers), vertex_buffers, &stride, &offset);

	engine.context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	engine.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	engine.context->IASetInputLayout(input_layout.Get());
	engine.context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	engine.context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	engine.context->DrawIndexed(n_indices, 0, 0);
}
