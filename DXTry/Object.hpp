#ifndef DXTRY_OBJECT_HPP
#define DXTRY_OBJECT_HPP

#include "includes.hpp"
#include "Error.hpp"

struct ObjectSerial {
	std::wstring_view vertex_path;
	std::wstring_view pixel_path;
	std::vector<D3D11_INPUT_ELEMENT_DESC> descriptors;
	std::vector<float> vertices;
	std::vector<USHORT> indices;

	Matrix model;
	UINT stride;
	UINT offset = 0;
};

struct Layout {
	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11InputLayout> input_layout;

	Layout() = default;
	Layout(const Layout&) = default;
	Layout(Layout&& other) = default;
	Layout(
		ComPtr<ID3D11VertexShader> _vertex_shader,
		ComPtr<ID3D11PixelShader> _pixel_shader,
		ComPtr<ID3D11InputLayout> _input_layout
	);

	Layout& operator=(Layout&& other) = default;
	Layout& operator=(const Layout&) = default;
};

struct Object: Layout {
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;
	ComPtr<ID3D11Buffer> constant_buffer;

	Matrix model;
	UINT n_indices;
	UINT stride;
	UINT offset;

	Object() = default;
	Object(Engine& engine, const ObjectSerial& serial);

	void init(Engine& engine, const ObjectSerial& serial);
	void update(Engine& engine);
	void render(Engine& engine);
	// void destroy(Engine& engine);

	static Layout create_layout(Engine& engine, const ObjectSerial& serial);
};

#endif // !DXTRY_OBJECT_HPP
