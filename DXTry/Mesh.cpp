#include "Error.hpp"
#include "Engine.hpp"
#include "Mesh.hpp"

Mesh::Mesh(MeshData&& data) : MeshData(std::move(data)) { }

Mesh::Mesh(Engine& engine, MeshData&& data) : MeshData(std::move(data)) {
	init(engine);
}

void Mesh::set(MeshData&& data) {
	MeshData::operator=(std::move(data));
}

void Mesh::init(Engine& engine) {
	auto[data, size] = engine.load_vertex(vertex_shader, vertex_path);
	check engine.device->CreateInputLayout(
		descriptors.data(),
		(UINT)descriptors.size(),
		data.get(), size,
		&input_layout
	);

	if (!geometry_path.empty())
		engine.load_geometry(geometry_shader, geometry_path);

	vertex_buffer = engine.create_buffer(D3D11_BIND_VERTEX_BUFFER, vertices);
	index_buffer = engine.create_buffer(D3D11_BIND_INDEX_BUFFER, indices);
}

void Mesh::update(Engine& engine) { }

void Mesh::render(Engine& engine) {
	ID3D11Buffer* vertex_buffers[] {
		vertex_buffer.Get()
	};
	engine.context->IASetVertexBuffers(0, (UINT)std::size(vertex_buffers), vertex_buffers, &stride, &offset);
	engine.context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	engine.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	engine.context->IASetInputLayout(input_layout.Get());
	engine.context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	engine.context->GSSetShader(geometry_shader.Get(), nullptr, 0);
	engine.context->DrawIndexed((UINT)indices.size(), 0, 0);
}
