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
	ID3D11VertexShader* vertex_shader = nullptr;
	ID3D11PixelShader* pixel_shader = nullptr;
	ID3D11InputLayout* input_layout = nullptr;

	Layout() = default;
	Layout(const Layout&) = delete;
	Layout(Layout&& other);
	Layout(
		ID3D11VertexShader* _vertex_shader,
		ID3D11PixelShader* _pixel_shader,
		ID3D11InputLayout* _input_layout
	);

	Layout& operator=(Layout&& other);
	Layout& operator=(const Layout&) = delete;

	void release() {
		safe_release(vertex_shader);
		safe_release(pixel_shader);
		safe_release(input_layout);
	}

	~Layout();
};

struct Object: Layout {
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

	Matrix model;
	UINT n_indices;
	UINT stride;
	UINT offset;

	Object() = default;
	Object(Engine& engine, const ObjectSerial& serial);

	void release();
	void update(Engine& engine, const ObjectSerial& serial);
	void draw(Engine& engine);

	static Layout create_layout(Engine& engine, const ObjectSerial& serial);

	~Object();
};

#endif // !DXTRY_OBJECT_HPP
