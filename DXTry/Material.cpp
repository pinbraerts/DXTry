#include "Engine.hpp"
#include "Material.hpp"

Material::Material(MaterialData&& data) : MaterialData(std::move(data)) { }

Material::Material(Engine& engine, MaterialData&& data) : MaterialData(std::move(data)) {
	init(engine);
}

void Material::set(MaterialData && data) {
	MaterialData::operator=(std::move(data));
}

void Material::init(Engine& engine) {
	pixel_shader = engine.load_pixel(pixel_path);
	if (light.diffuse != Vector4::Zero ||
		light.ambient != Vector4::Zero ||
		light.specular != Vector4::Zero ||
		light.shininess != Vector4::Zero)
		constant_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, &light, sizeof(light));

	if (!texture_path.empty()) {
		auto [ x, y ] = engine.create_texture(texture_path);
		texture = std::move(x);
		texture_view = std::move(y);

		D3D11_SAMPLER_DESC desc {
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_TEXTURE_ADDRESS_MIRROR, // u
			D3D11_TEXTURE_ADDRESS_MIRROR, // v
			D3D11_TEXTURE_ADDRESS_MIRROR, // w
			0, // LOD bias
			0, // anisotropy
			D3D11_COMPARISON_NEVER,
			{ 0, 0, 0, 0 }, // border color
			0, // min LOD
			2 // max LOD
		};
		check engine.device->CreateSamplerState(&desc, &sampler);
	}
}

void Material::update(Engine& engine) { }

void Material::render(Engine& engine) {
	if (constant_buffer != nullptr) {
		ID3D11Buffer* pscbs[] = { constant_buffer.Get() };
		engine.context->PSSetConstantBuffers(3, (UINT)std::size(pscbs), pscbs);
	}
	if (texture != nullptr) {
		ID3D11ShaderResourceView* views[]{
			texture_view.Get()
		};
		engine.context->PSSetShaderResources(0, (UINT)std::size(views), views);

		ID3D11SamplerState* samplers[]{
			sampler.Get()
		};
		engine.context->PSSetSamplers(0, (UINT)std::size(samplers), samplers);
	}
	engine.context->PSSetShader(pixel_shader.Get(), nullptr, 0);
}
