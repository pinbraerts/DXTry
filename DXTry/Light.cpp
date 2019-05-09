#include "Light.hpp"
#include "Engine.hpp"

void Light::set(ObjectData&& data, LightData l) {
	Object::set(std::move(data));
	LightData::operator=(l);
}

void Light::init(Engine& engine) {
	Object::init(engine);
	constant_light = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(LightData));
}

void Light::update(Engine& engine) {
	Object::update(engine);
	engine.context->UpdateSubresource(
		constant_light.Get(),
		0,
		nullptr,
		static_cast<LightData*>(this),
		0,
		0
	);
}

void Light::render(Engine& engine) {
	ID3D11Buffer* pscbs[] {
		constant_light.Get()
	};
	engine.context->VSSetConstantBuffers(1, (UINT)std::size(pscbs), pscbs);
	engine.context->PSSetConstantBuffers(1, (UINT)std::size(pscbs), pscbs);

	if(position.w < 0.5f)
		Object::render(engine);
}
