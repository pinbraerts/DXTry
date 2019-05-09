#ifndef DXTRY_MATERIAL_HPP
#define DXTRY_MATERIAL_HPP

#include "IObject.hpp"

struct MaterialData {
	std::wstring_view pixel_path;
	std::wstring_view texture_path;

	struct LightConstant {
		Vector4 ambient;
		Vector4 diffuse;
		Vector4 specular;
		Vector4 shininess;
	} light;
};

struct Material : IObject, MaterialData {
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11Buffer> constant_buffer;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> texture_view;
	ComPtr<ID3D11SamplerState> sampler;

	Material() = default;
	Material(MaterialData&& data);
	Material(Engine& engine, MaterialData&& data);

	void set(MaterialData&& data);

	void init(Engine& engine) override;
	void update(Engine& engine) override;
	void render(Engine& engine) override;
};

#endif // !DXTRY_MATERIAL_HPP
