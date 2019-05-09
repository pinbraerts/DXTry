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
		geometry_shader = engine.load_geometry(geometry_path);

	if(!vertices.empty())
		vertex_buffer = engine.create_buffer(D3D11_BIND_VERTEX_BUFFER, vertices);

	if(!indices.empty())
		index_buffer = engine.create_buffer(D3D11_BIND_INDEX_BUFFER, indices);
}

void Mesh::update(Engine& engine) { }

void Mesh::render(Engine& engine) {
	ID3D11Buffer* vbuffers[] {
		vertex_buffer.Get()
	};
	engine.context->IASetVertexBuffers(0, (UINT)std::size(vbuffers), vbuffers, &stride, &offset);

	engine.context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	engine.context->IASetInputLayout(input_layout.Get());
	engine.context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	engine.context->GSSetShader(geometry_shader.Get(), nullptr, 0);
	if (index_buffer != nullptr) {
		engine.context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		engine.context->DrawIndexed((UINT)indices.size(), 0, 0);
	}
	else {
		engine.context->Draw(sizeof(float) * (UINT)vertices.size() / stride, 0);
	}
}
