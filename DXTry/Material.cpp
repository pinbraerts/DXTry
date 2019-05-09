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
	engine.load_pixel(pixel_shader, pixel_path);
	if (light.diffuse != Vector4::Zero ||
		light.ambient != Vector4::Zero ||
		light.specular != Vector4::Zero ||
		light.shininess != Vector4::Zero)
		constant_buffer = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, &light, sizeof(light));
}

void Material::update(Engine& engine) { }

void Material::render(Engine& engine) {
	if (constant_buffer != nullptr) {
		ID3D11Buffer* pscbs[] = { constant_buffer.Get() };
		engine.context->PSSetConstantBuffers(3, (UINT)std::size(pscbs), pscbs);
	}
	engine.context->PSSetShader(pixel_shader.Get(), nullptr, 0);
}
