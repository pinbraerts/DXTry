#ifndef DXTRY_MATERIAL_HPP
#define DXTRY_MATERIAL_HPP

#include "IObject.hpp"

struct MaterialData {
	std::wstring_view pixel_path;

	struct {
		Vector4 ambient;
		Vector4 shininess;
	} light;

	struct LightConstant2 {
		Vector4 diffuse;
		Vector4 specular;
	};
	struct Path {
		std::wstring_view diffuse;
		std::wstring_view specular;
	};

	union {
		LightConstant2 light2;
		Path path;
	};
};

struct Material: IObject, MaterialData {
	ComPtr<ID3D11PixelShader> pixel_shader;
	ComPtr<ID3D11Buffer> constant_buffer;

	ComPtr<ID3D11Texture2D> textures[2];
	ComPtr<ID3D11ShaderResourceView> views[2];
	ComPtr<ID3D11SamplerState> sampler; // use same for two textures

	Material() = default;
	Material(MaterialData&& data);
	Material(Engine& engine, MaterialData&& data);

	void set(MaterialData&& data);

	void init(Engine& engine) override;
	void update(Engine& engine) override;
	void render(Engine& engine) override;
};

#endif // !DXTRY_MATERIAL_HPP
