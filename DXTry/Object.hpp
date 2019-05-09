#ifndef DXTRY_OBJECT_HPP
#define DXTRY_OBJECT_HPP

#include "includes.hpp"
#include "Error.hpp"

struct MaterialSerial {
	std::wstring_view vertex_path;
	std::wstring_view pixel_path;
	std::vector<D3D11_INPUT_ELEMENT_DESC> descriptors;

	struct Constant {
		float ambient_strength;
		float diffuse_strength;
		float specular_strength;
		float shininess;
	} constant;

	std::wstring_view geometry_path;
};

struct ObjectSerial {
	MaterialSerial material;
	std::vector<float> vertices;
	std::vector<USHORT> indices;

	Matrix model;
	UINT stride;
	UINT offset = 0;
};

struct Material {
	ComPtr<ID3D11VertexShader> vertex_shader;
	ComPtr<ID3D11GeometryShader> geometry_shader;
	//ComPtr<ID3D11Buffer> stream_output;
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11InputLayout> input_layout;

	MaterialSerial::Constant material;

	Material() = default;
	Material(Engine& engine, const MaterialSerial& serial);

	void init(Engine& engine, const MaterialSerial& serial);
};

struct Object: Material {
	// ComPtr
	ComPtr<ID3D11Buffer> vertex_buffer;
	ComPtr<ID3D11Buffer> index_buffer;
	ComPtr<ID3D11Buffer> constant_buffers[2];

	// Constants
	Matrix model;

	// Other stuff
	UINT n_indices;
	UINT stride;
	UINT offset;

	Object() = default;
	Object(Engine& engine, const ObjectSerial& serial);

	void init(Engine& engine, const ObjectSerial& serial);
	void update(Engine& engine);
	void render(Engine& engine);
	// void destroy(Engine& engine);
};

#endif // !DXTRY_OBJECT_HPP
