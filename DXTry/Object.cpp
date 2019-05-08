#include "Object.hpp"
#include "Engine.hpp"

Layout::Layout(
	ID3D11VertexShader* _vertex_shader,
	ID3D11PixelShader* _pixel_shader,
	ID3D11InputLayout* _input_layout
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

void Object::release() {
	Layout::release();
	safe_release(vertex_buffer);
	safe_release(index_buffer);
	model = Matrix();
	n_indices = 0;
	stride = 0;
	offset = 0;
}

void Object::update(Engine& engine, const ObjectSerial& serial) {
	release();
	Layout::operator=(create_layout(engine, serial));
	vertex_buffer = engine.create_buffer(D3D11_BIND_VERTEX_BUFFER, serial.vertices);
	index_buffer = engine.create_buffer(D3D11_BIND_INDEX_BUFFER, serial.indices);
	model = serial.model;
	n_indices = (UINT)serial.indices.size();
	stride = serial.stride;
	offset = serial.offset;
}

Layout Object::create_layout(Engine& engine, const ObjectSerial& serial) {
	Layout res;
	std::string_view shb = Engine::load_vertex(
		engine.device.Get(),
		res.vertex_shader,
		serial.vertex_path
	);

	HRESULT hr = engine.device->CreateInputLayout(
		serial.descriptors.data(),
		(UINT)serial.descriptors.size(),
		shb.data(),
		shb.size(),
		&res.input_layout
	);
	if (FAILED(hr))
		throw Error(hr);

	delete[] shb.data();

	delete[] Engine::load_pixel(
		engine.device.Get(),
		res.pixel_shader,
		serial.pixel_path
	).data();
	return res;
}

void Object::draw(Engine& engine) {
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
	engine.context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	engine.context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);
	engine.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	engine.context->IASetInputLayout(input_layout);
	engine.context->VSSetShader(vertex_shader, nullptr, 0);
	engine.context->VSSetConstantBuffers(0, 1, engine.constant_buffer.GetAddressOf());
	engine.context->PSSetShader(pixel_shader, nullptr, 0);
	engine.context->DrawIndexed(n_indices, 0, 0);
}

Object::~Object() {
	release();
}

Layout::Layout(Layout && other):
	vertex_shader(std::exchange(other.vertex_shader, nullptr)),
	pixel_shader(std::exchange(other.pixel_shader, nullptr)), 
	input_layout(std::exchange(other.input_layout, nullptr)) {}

Layout& Layout::operator=(Layout&& other) {
	if (&other == this)
		return *this;
	vertex_shader = std::exchange(other.vertex_shader, nullptr);
	pixel_shader = std::exchange(other.pixel_shader, nullptr);
	input_layout = std::exchange(other.input_layout , nullptr);
	return *this;
}

Layout::~Layout() {
	release();
}
