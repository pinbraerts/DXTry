#ifndef DXTRY_MESH_HPP
#define DXTRY_MESH_HPP

#include "IObject.hpp"

struct MeshData {
	std::wstring_view vertex_path;
	std::wstring_view geometry_path;
	std::vector<D3D11_INPUT_ELEMENT_DESC> descriptors;

	std::vector<float> vertices;
	std::vector<USHORT> indices;
	UINT stride;
	UINT offset = 0;
};

struct Mesh : IObject, MeshData {
	Mesh() = default;
	Mesh(MeshData&& data);
	Mesh(Engine& engine, MeshData&& data);

	void set(MeshData&& data);

	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11GeometryShader> geometry_shader;
	ComPtr<ID3D11InputLayout> input_layout;
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;

	void init(Engine& engine) override;
	void update(Engine& engine) override;
	void render(Engine& engine) override;
};

#endif // !DXTRY_MESH_HPP
